#include "mini_db.hpp"
#include <csignal>
#include <cstdlib>
#include <fstream>

// crée le socket et prépare l'adresse d'écoute (127.0.0.1:port)
Socket::Socket(int port) : fd(socket(AF_INET, SOCK_STREAM, 0))
{
	if (fd == -1)
		throw std::runtime_error("Socket creation failed");
	int reuse = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);
}

// ferme le socket d'écoute
Socket::~Socket()
{
	if (fd != -1)
		close(fd);
}

// bind puis listen sur le port donné
void Socket::bindAndListen()
{
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("Socket bind failed");
	if (listen(fd, 10) < 0)
		throw std::runtime_error("Socket listen failed");
}

// accepte une connexion entrante, renvoie le fd du client
int Socket::acceptClient()
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0)
		throw std::runtime_error("Failed to accept connection");
	return client_fd;
}

// lit un morceau de données envoyées par un client (vide si déconnexion)
std::string Socket::receiveChunk(int client_fd)
{
	char buffer[1024];
	int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read <= 0)
		return std::string();
	buffer[bytes_read] = '\0';
	return std::string(buffer);
}

Server::Server(int port, std::map<std::string, std::string>& db)
	: listening_socket(port), db(db)
{
	FD_ZERO(&active_fds);
}

// traite une commande complète (une ligne) et répond au client
void Server::handleCommand(int client_fd, const std::string& line)
{
	std::istringstream stream(line);
	std::string command, key, value;
	stream >> command >> key >> value;

	if (command == "POST" && !value.empty())
	{
		db[key] = value;
		send(client_fd, "0\n", 2, 0);
	}
	else if (command == "GET" && value.empty())
	{
		std::map<std::string, std::string>::iterator it = db.find(key);
		if (it == db.end())
			send(client_fd, "1\n", 2, 0);
		else
		{
			std::string reply = "0 " + it->second + "\n";
			send(client_fd, reply.c_str(), reply.size(), 0);
		}
	}
	else if (command == "DELETE" && value.empty())
	{
		std::map<std::string, std::string>::iterator it = db.find(key);
		if (it == db.end())
			send(client_fd, "1\n", 2, 0);
		else
		{
			db.erase(it);
			send(client_fd, "0\n", 2, 0);
		}
	}
	else
		send(client_fd, "2\n", 2, 0);
}

// découpe les données reçues en lignes complètes, traite chacune
// (une connexion persistante peut envoyer plusieurs commandes d'un coup,
// et une commande peut aussi arriver en plusieurs morceaux : on bufferise
// par client et on ne traite que ce qui est déjà terminé par un '\n')
void Server::handleIncomingData(int client_fd, const std::string& chunk)
{
	pending_input[client_fd] += chunk;
	size_t newline_pos;
	while ((newline_pos = pending_input[client_fd].find('\n')) != std::string::npos)
	{
		std::string line = pending_input[client_fd].substr(0, newline_pos);
		pending_input[client_fd].erase(0, newline_pos + 1);
		handleCommand(client_fd, line);
	}
}

// ferme proprement la connexion d'un client
void Server::disconnectClient(int client_fd)
{
	FD_CLR(client_fd, &active_fds);
	pending_input.erase(client_fd);
	close(client_fd);
}

// boucle principale : accept + lecture des clients via select()
int Server::run()
{
	try
	{
		listening_socket.bindAndListen();
		FD_SET(listening_socket.fd, &active_fds);
		max_fd = listening_socket.fd;

		std::cout << "ready" << std::endl;

		while (true)
		{
			fd_set read_fds = active_fds;
			if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0)
				throw std::runtime_error("Failed to select");

			for (int fd = 0; fd <= max_fd; ++fd)
			{
				if (!FD_ISSET(fd, &read_fds))
					continue;
				if (fd == listening_socket.fd)
				{
					int client_fd = listening_socket.acceptClient();
					FD_SET(client_fd, &active_fds);
					if (client_fd > max_fd)
						max_fd = client_fd;
				}
				else
				{
					std::string chunk = listening_socket.receiveChunk(fd);
					if (chunk.empty())
						disconnectClient(fd);
					else
						handleIncomingData(fd, chunk);
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error during server run: " << e.what() << std::endl;
		return 1;
	}
}

static std::string save_path;
static std::map<std::string, std::string> db;

// sauvegarde la base sur disque et quitte proprement à la réception de SIGINT
static void handleSigint(int)
{
	std::ofstream file(save_path.c_str());
	if (file.is_open())
	{
		for (std::map<std::string, std::string>::iterator it = db.begin(); it != db.end(); ++it)
			file << it->first << " " << it->second << "\n";
		file.close();
		/* exit() skips destructors of local objects: without this explicit
		   close(), the ofstream's buffer would never reach disk */
	}
	exit(0);
}

// charge la base depuis le fichier de sauvegarde s'il existe
static void loadDb()
{
	std::ifstream file(save_path.c_str());
	std::string key, value;
	while (file >> key >> value)
		db[key] = value;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Wrong number of arguments" << std::endl;
		return EXIT_FAILURE;
	}
	save_path = argv[2];
	signal(SIGINT, handleSigint);
	loadDb();
	try
	{
		Server server(atoi(argv[1]), db);
		return server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Internal server error" << std::endl;
		return EXIT_FAILURE;
	}
}
