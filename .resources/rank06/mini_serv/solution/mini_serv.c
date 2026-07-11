#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

// un client par fd : son id, et ses tampons d'entrée/sortie en attente
typedef struct s_client
{
	int		id;
	char	*buf_in;
	char	*buf_out;
}	t_client;

// tableau indexé par fd (FD_SETSIZE = la limite de select() elle-même,
// donc pas besoin d'inventer une constante ou d'utiliser #define)
static t_client	clients[FD_SETSIZE];
static fd_set	active_fds;
static int		max_fd;
static int		next_id;

// affiche le message d'erreur imposé par le sujet et quitte
static void	fatal_error(void)
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

// concatène add à la fin de buf (buf peut être NULL), libère l'ancien buf,
// renvoie la nouvelle chaîne allouée
static char	*str_join(char *buf, char *add)
{
	char	*result;

	result = malloc((buf ? strlen(buf) : 0) + strlen(add) + 1);
	if (!result)
		fatal_error();
	result[0] = '\0';
	if (buf)
		strcpy(result, buf);
	strcat(result, add);
	free(buf);
	return (result);
}

// extrait la première ligne complète (avec son '\n') du tampon *buf,
// et laisse le reste dans *buf ; renvoie NULL si aucune ligne n'est complète
static char	*extract_line(char **buf)
{
	char	*newline;
	char	*line;
	char	*rest;
	size_t	line_len;
	size_t	i;

	if (!*buf)
		return (NULL);
	newline = strstr(*buf, "\n");
	if (!newline)
		return (NULL);
	line_len = (size_t)(newline - *buf) + 1;
	line = malloc(line_len + 1);
	if (!line)
		fatal_error();
	i = 0;
	while (i < line_len)
	{
		line[i] = (*buf)[i];
		i++;
	}
	line[line_len] = '\0';
	rest = str_join(NULL, newline + 1);
	free(*buf);
	*buf = rest;
	return (line);
}

// ajoute msg au tampon de sortie de chaque client connecté sauf except_fd
static void	broadcast_to_all_except(int except_fd, char *msg)
{
	int	fd;

	for (fd = 0; fd <= max_fd; fd++)
		if (fd != except_fd && FD_ISSET(fd, &active_fds))
			clients[fd].buf_out = str_join(clients[fd].buf_out, msg);
}

// envoie ce qui peut l'être sans bloquer (select nous a dit que c'est prêt),
// garde le reste pour le prochain tour si l'envoi est partiel
static void	flush_output(int fd)
{
	size_t	remaining;
	int		sent;
	char	*rest;

	remaining = strlen(clients[fd].buf_out);
	sent = send(fd, clients[fd].buf_out, remaining, 0);
	if (sent <= 0)
		return ;
	if ((size_t)sent == remaining)
	{
		free(clients[fd].buf_out);
		clients[fd].buf_out = NULL;
	}
	else
	{
		rest = str_join(NULL, clients[fd].buf_out + sent);
		free(clients[fd].buf_out);
		clients[fd].buf_out = rest;
	}
}

// ferme un client, libère ses tampons, prévient les autres clients
static void	disconnect_client(int fd)
{
	char	msg[64];

	close(fd);
	FD_CLR(fd, &active_fds);
	sprintf(msg, "server: client %d just left\n", clients[fd].id);
	free(clients[fd].buf_in);
	free(clients[fd].buf_out);
	clients[fd].buf_in = NULL;
	clients[fd].buf_out = NULL;
	broadcast_to_all_except(fd, msg);
}

// accepte une nouvelle connexion, lui donne un id, prévient les autres clients
static void	accept_client(int server_fd)
{
	int		fd;
	char	msg[64];

	fd = accept(server_fd, NULL, NULL);
	if (fd < 0 || fd >= FD_SETSIZE)
	{
		if (fd >= 0)
			close(fd);
		return ;
	}
	if (fd > max_fd)
		max_fd = fd;
	FD_SET(fd, &active_fds);
	clients[fd].id = next_id++;
	clients[fd].buf_in = NULL;
	clients[fd].buf_out = NULL;
	sprintf(msg, "server: client %d just arrived\n", clients[fd].id);
	broadcast_to_all_except(fd, msg);
}

// une ligne reçue de fd doit être renvoyée à tous les autres, préfixée
static void	broadcast_line(int fd, char *line)
{
	char	prefix[32];
	char	*full;

	sprintf(prefix, "client %d: ", clients[fd].id);
	full = str_join(NULL, prefix);
	full = str_join(full, line);
	broadcast_to_all_except(fd, full);
	free(full);
}

// lit ce qui est disponible sur fd ; déconnecte le client si recv échoue,
// sinon découpe les données reçues en lignes complètes et les diffuse
static void	handle_read(int fd)
{
	char	buffer[4096];
	int		bytes_read;
	char	*line;

	bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read <= 0)
	{
		disconnect_client(fd);
		return ;
	}
	buffer[bytes_read] = '\0';
	clients[fd].buf_in = str_join(clients[fd].buf_in, buffer);
	while ((line = extract_line(&clients[fd].buf_in)))
	{
		broadcast_line(fd, line);
		free(line);
	}
}

// crée, bind et écoute le socket serveur sur 127.0.0.1:port
static int	create_server_socket(int port)
{
	int					server_fd;
	struct sockaddr_in	address;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		fatal_error();
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		fatal_error();
	if (listen(server_fd, 128) < 0)
		fatal_error();
	return (server_fd);
}

int	main(int argc, char **argv)
{
	int		server_fd;
	fd_set	read_fds;
	fd_set	write_fds;
	int		fd;

	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	server_fd = create_server_socket(atoi(argv[1]));
	FD_ZERO(&active_fds);
	FD_SET(server_fd, &active_fds);
	max_fd = server_fd;
	while (1)
	{
		read_fds = active_fds;
		FD_ZERO(&write_fds);
		for (fd = 0; fd <= max_fd; fd++)
			if (fd != server_fd && FD_ISSET(fd, &active_fds) && clients[fd].buf_out)
				FD_SET(fd, &write_fds);
		if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
			continue ;
		for (fd = 0; fd <= max_fd; fd++)
		{
			if (fd == server_fd && FD_ISSET(fd, &read_fds))
				accept_client(server_fd);
			else if (FD_ISSET(fd, &read_fds))
				handle_read(fd);
			if (fd != server_fd && FD_ISSET(fd, &write_fds) && FD_ISSET(fd, &active_fds))
				flush_output(fd);
		}
	}
}
