#include "common.hpp"

#define SERVER_PORT 18000


int	main()
{
	int 				listenfd, connfd, n;
	struct sockaddr_in	servaddr;
	uint8_t				buff[MAXLINE + 1];
	uint8_t				recvline[MAXLINE + 1];

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		std::cout << "socket inti error" << std::endl;
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port =	 htons(SERVER_PORT);

	if ((bind(listenfd, (SA *)&servaddr, sizeof(servaddr))) < 0)
	{
		std::cout << "bind error" << std::endl;
		exit(EXIT_FAILURE);
	}

	if ((listen(listenfd, 10)) < 0 )
	{
		std::cout << "listen error" << std::endl;
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		std::cout << "waiting for connection on port " << SERVER_PORT << std::endl;
		connfd = accept(listenfd, (SA*)NULL, NULL);
		memset(recvline, 0, MAXLINE);

		while (1)
		{
			;
		}
	}

	return (0);
}
