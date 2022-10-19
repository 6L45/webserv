#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <csignal>
#include "webserv.hpp"

void	Webserv::__bind_and_listen(void)
{
	this->_domain = AF_INET;
	this->_service = SOCK_STREAM;
	this->_protocol = 0;
	this->_port = SERVER_PORT;
	this->_interface = INADDR_ANY;

	this->_address2.sin_family = this->_domain;
	this->_address2.sin_port = htons(this->_port);
	this->_address2.sin_addr.s_addr = htonl(this->_interface);
	this->_sock2 = socket(this->_domain, this->_service, this->_protocol);

	// bind socket to a port
	if ( (bind(this->_sock2, (struct sockaddr *)&(this->_address2), 
					sizeof(this->_address2))) < 0 )
	{
		std::cerr << "Error : bind socket2" << std::endl;
		perror("");
		exit(EXIT_FAILURE);
	}
	// listen to the port
	if ( (listen(this->_sock2, 10)) < 0 ) // <--- second arg = backlog : defines the maximum length to which the queue of pending connections for sockfd may grow
	{
		std::cerr << "Error : listen2" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "connected and listening sock2" << std::endl;
}

Webserv::Webserv()
{
	// hard coded for now. can be passed as parameters at initialization
	this->_domain = AF_INET;
	this->_service = SOCK_STREAM;
	this->_protocol = 0;
	this->_port = PORT;
	this->_interface = INADDR_ANY;

	this->_address.sin_family = this->_domain;
	this->_address.sin_port = htons(this->_port);
	this->_address.sin_addr.s_addr = htonl(this->_interface);
	// -----------------------------------------------------------------

	// socket creation + make it non blocking
	this->_sock = socket(this->_domain, this->_service, this->_protocol);

	// bind socket to a port
	if ( (bind(this->_sock, (struct sockaddr *)&(this->_address), 
					sizeof(this->_address))) < 0 )
	{
		std::cerr << "Error : bind socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	// listen to the port
	if ( (listen(this->_sock, 10)) < 0 ) // <--- second arg = backlog : defines the maximum length to which the queue of pending connections for sockfd may grow
	{
		std::cerr << "Error : listen" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "connected and listening" << std::endl;
	this->__bind_and_listen();
}

void	ft_putstr_fd(int fd, const char *str)
{
	while (*str)
		write(fd, str++, 1);
}

void 	Webserv::launch()
{
	struct sockaddr_in	addr;
	socklen_t			addr_len;
	int 				send_ret;

	struct sockaddr_in client;
	socklen_t client_len;
	uint16_t			conn_port;

	// collection of file descriptors
	fd_set				current_sockets, ready_sockets;
	 
	FD_ZERO(&current_sockets);
	FD_SET(this->_sock, &current_sockets);
	FD_SET(this->_sock2, &current_sockets);
	 
	std::cout << "++waiting for connection ++" << std::endl; 

	// SERVER LOOP
	while (1)
	{
		// cause FD_SET() is destructive 
		ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) // 1st param = size // 2nd = fd to check for reading // 3rd = fd to check for writing // 4th = error // 5th time
		{
			perror("select error");
			exit(EXIT_FAILURE);
		}
		//FD_SETSIZE() max size possible
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			// if (i != this->_sock && FD_ISSET(i, &current_sockets))
			// 	std::cout << "++connexion still open with this client : " << i << "++" << std::endl;
			// if on fd in select is trigger
			if (FD_ISSET(i, &ready_sockets))
			{
				// if reacting fd is this->_sock = new connection
				if (i == this->_sock)
				{
					std::cout << "++connexion request on socket fd : " << i << "++" << std::endl;
					// accept and add new connection to current_set
					this->_confd = accept(this->_sock, (struct sockaddr *)&addr, &addr_len);
					if (this->_confd < 0)
					{
						perror("accept1 error");
						exit(EXIT_FAILURE);
					}

					getsockname(this->_sock, (SA *)&client, &client_len);
					conn_port = ntohs(client.sin_port);
					std::cout << "++client connected by port : " << conn_port << " ++" << std::endl;
					FD_SET(this->_confd, &current_sockets);
					std::cout << "++Connexion accepted, client fd : " << this->_confd << "++" << std::endl;
				}
				else if (i == this->_sock2)
				{
					std::cout << "++connexion request on socket fd : " << i << "++" << std::endl;
					// accept and add new connection to current_set
					this->_confd = accept(this->_sock2, (struct sockaddr *)&addr, &addr_len);
					if (this->_confd < 0)
					{
						perror("accept2 error");
						exit(EXIT_FAILURE);
					}

					getsockname(this->_sock2, (SA *)&client, &client_len);
					conn_port = ntohs(client.sin_port);
					std::cout << "++client connected by port : " << conn_port << " ++" << std::endl;
					FD_SET(this->_confd, &current_sockets);
					std::cout << "++Connexion accepted, client fd : " << this->_confd << "++" << std::endl;
				}
				else
				{
					std::cout << "++client request on socket fd : " << i << "++" << std::endl;
					this->_confd = i;
					__print_request_client(); // Si on ne lit pas select va trigger en boucle car le client fd est pret a être lu. En plus de ça la requete est envoyé ligne par ligne on dirait

					std::string	server_message = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: 55\r\n\
Keep-Alive: timeout=5, max=1000\r\n\
Connection: Keep-Alive\r\n\
\r\n\
1234567890123456789012345678901234567890123456789012345"; 

					std::signal(SIGPIPE, SIG_IGN); //ignorer le sigpipe car sinon crash
					if (( send_ret = send(i, server_message.c_str(), server_message.length(), 0)) < 0)
					{
						if (errno == EPIPE && i != this->_sock)
						{
							std::cout << "++connexion with client is lost, closing fd : " << i << "++" << std::endl;
							FD_CLR(i, &current_sockets);
							close(i);
						}
					}
					else
						std::cout << "Message send to the client fd : " << i << "++" << std::endl;				}
			}
		}
	}
}

void	Webserv::__print_request_client()
{
	int	n;

	this->_recvline = static_cast<char *>(malloc(MAXLINE));
	memset(this->_recvline, 0, MAXLINE);
	while ( (n = recv(this->_confd, this->_recvline, MAXLINE - 1, MSG_DONTWAIT)) > 0 )
	{
		std::cout << this->_recvline << std::endl << std::endl;
//		if (this->_recvline[n - 1] == '\n')
//			break;
	}
	if (n < 0)
	{
		if (errno != EAGAIN)
		{
			perror("read error");
			close(this->_confd);
		}
	}
	free(this->_recvline);
}
