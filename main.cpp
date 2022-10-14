#include "common.hpp"

int guard(int n) { if (n == -1) { exit(1); } return n; }

void	listen_ft()
{
	struct	sockaddr_in	_address;

	_address.sin_family = AF_INET;
	_address.sin_port = htons(18000);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);

	//int sock = socket(AF_INET, SOCK_STREAM, 0);
	int sock = guard(socket(AF_INET, SOCK_STREAM, 0));	
	int flags = guard(fcntl(sock, F_GETFL));
	guard(fcntl(sock, F_SETFL, flags | O_NONBLOCK));

	if ( (bind(sock, (struct sockaddr *)&(_address), sizeof(_address))) < 0 )
	{
		std::cout << "bind socket error" << std::endl;
		std::cout << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	if ((listen(sock, 10)) < 0)
	{
		std::cout << "listen error" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	
	while (1)
	{
		int confd = accept(sock, (struct sockaddr *)NULL, NULL);
		if (confd == -1)
		{
			if (errno == EWOULDBLOCK)
				std::cout << "tourner dans le vide, tourner tourner dans le vide" << std::endl;
			sleep(1);
		}
		else
		{
			std::cout << confd << std::endl;
		}
	}
}

int main()
{
//	Webserv test(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
	Webserv test;

	test.launch();
//	listen_ft();
	return 0;
}

