#include <arpa/inet.h>
#include <sys/select.h>

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
	int	flags = fcntl(this->_sock, F_GETFL);
	fcntl(this->_sock, F_SETFL, flags | O_NONBLOCK);

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
}

void 	Webserv::launch()
{
	struct sockaddr_in	addr;
	socklen_t			addr_len;
	
	// collection of file descriptors
	fd_set				current_sockets, ready_sockets;
	 
	//init fd set
	FD_ZERO(&current_sockets);
	
	// add this->_sock (binded socket) to current_sockets fd set
	FD_SET(this->_sock, &current_sockets);
	 
	std::cout << "waiting for connection on port " << PORT << std::endl; 

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
			// if on fd in select is trigger
			if (FD_ISSET(i, &ready_sockets))
			{
				// if reacting fd is this->_sock = new connection
				if (i == this->_sock)
				{
					// accept and add new connection to current_set
					this->_confd = accept(this->_sock, (struct sockaddr *)NULL, NULL);
					FD_SET(this->_confd, &current_sockets);
				}
				// else handle request
				else
				{
					std::string	server_message = "HTTP/1.1 200 OK\r\n\
Content-Length: 55\r\n\
Content-Type: text/html\r\n\
Last-Modified: Wed, 12 Aug 1998 15:03:50 GMT\r\n\
Accept-Ranges: bytes\r\n\
ETag: “04f97692cbd1:377”\r\n\
Date: Thu, 19 Jun 2008 19:29:07 GMT\r\n\
\r\n\
1234567890123456789012345678901234567890123456789012345"; 

					std::cout << "-------------------------------- " << this->_confd << " ----------------------------------" << std::endl;
					send(this->_confd, server_message.c_str(), server_message.length(), 0);

					// ajouter par la un if (connection lost on this fd)
					// close fd
					// and clear fd dans tous les cas
					FD_CLR(i, &current_sockets);
					
				}
			}
		}
	}
}

void	Webserv::print_request_client()
{
	int	n;

	this->_recvline = static_cast<char *>(malloc(MAXLINE));
	memset(this->_recvline, 0, MAXLINE);
	while ( (n = read(this->_confd, this->_recvline, MAXLINE - 1)) > 0 )
	{
		std::cout << this->_recvline << std::endl << std::endl;
		if (this->_recvline[n - 1] == '\n')
			break;
	}
	if (n < 0)
	{
		std::cout << "read error" << std::endl;
		close(this->_confd);
	}
	free(this->_recvline);
}









/*
std::snprintf((char *)_buff, sizeof(_buff), "HTTP/1.0 200 OK\r\n\r\nHELLO WORLD!"); 
		write(_confd, (char *)_buff, strlen((char *)_buff));
*/