#ifndef MINI_DB_HPP
#define MINI_DB_HPP

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>
#include <sstream>
#include <string>

// wrappe un socket TCP serveur : création, bind/listen, accept, lecture
class Socket
{
	private:
		struct sockaddr_in address;

	public:
		int fd;

		// crée le socket et prépare l'adresse d'écoute (127.0.0.1:port)
		Socket(int port);
		// ferme le socket d'écoute
		~Socket();
		// bind puis listen sur le port donné
		void bindAndListen();
		// accepte une connexion entrante, renvoie le fd du client
		int acceptClient();
		// lit un morceau de données envoyées par un client (vide si déconnexion)
		std::string receiveChunk(int client_fd);
};

// boucle select(), gère les clients connectés et la base clé-valeur
class Server
{
	private:
		Socket listening_socket;
		fd_set active_fds;
		int max_fd;
		std::map<std::string, std::string>& db;
		// tampon par client : accumule les octets reçus jusqu'à une ligne complète
		std::map<int, std::string> pending_input;

		// traite une commande complète (une ligne) et répond au client
		void handleCommand(int client_fd, const std::string& line);
		// découpe les données reçues en lignes complètes, traite chacune
		void handleIncomingData(int client_fd, const std::string& chunk);
		// ferme proprement la connexion d'un client
		void disconnectClient(int client_fd);

	public:
		Server(int port, std::map<std::string, std::string>& db);
		// boucle principale : accept + lecture des clients via select()
		int run();
};

#endif
