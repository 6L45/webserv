#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void	fatal_error(std::string msg, int exit_status)
{
	std::cerr << "webserv: " << msg << std::endl;
	exit(exit_status);
}

int main(int ac, char **av)
{
	int	port;
	int	sock;
	struct sockaddr_in sa;

	if (ac != 2)
		fatal_error("usage: webserv <int:port>", 1);
	std::string	s_port(av[1]);
	port = std::stoi(s_port);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		fatal_error("unable to create the socket", 3);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	connect(sock, (struct sockaddr *)&sa, (socklen_t)sizeof(sa));
	std::string msg("Hello ITS mE");
	send(sock, msg.c_str(), msg.length(), 0);
	close(sock);
}