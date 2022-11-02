#include "server.class.hpp"

Server::Server(int port)
{
	this->_domain =		AF_INET;
	this->_service =	SOCK_STREAM;
	this->_protocol =	0;
	this->_port =		port;
	this->_interface =	INADDR_ANY;

	this->_address.sin_family =			this->_domain; //AF_INET
	this->_address.sin_port =			htons(this->_port);
	this->_address.sin_addr.s_addr =	htonl(this->_interface); // INADRR_ANY

	// socket creation 
	this->_sock = socket(this->_domain, this->_service, this->_protocol);

	// bind socket to a port
	if ( (bind(this->_sock, (struct sockaddr *)&(this->_address), 
					sizeof(this->_address))) < 0 )
	{
		perror("Error : bind socket");
		exit(EXIT_FAILURE);
	}

	// listen to the port
	if ( (listen(this->_sock, 10)) < 0 ) // <--- second arg = backlog : defines the maximum length to which the queue of pending connections for sockfd may grow
	{
		perror("Error : listen");
		exit(EXIT_FAILURE);
	}

	std::cout << "connected and listening : " << this->_sock << std::endl;
}

int	&Server::get_socket()
{
	return (this->_sock);
}

void	Server::print_request_client(int fd)
{
	int	n;

	this->_recvline = static_cast<char *>(malloc(MAXLINE));
	memset(this->_recvline, 0, MAXLINE);
	std::cout << std::endl;

	while ( (n = recv(fd, this->_recvline, MAXLINE - 1, MSG_DONTWAIT)) > 0 );

	if (n < 0)
	{
		if (errno != EAGAIN)
		{
			perror("read error");
			close(fd); //need to delete in fds in webserv the fd in map? to avoid select error..
		}
	}

	std::string	request(this->_recvline);
	free(this->_recvline);

	Http_handler request_handler(request);
}

void	Server::send_response(int fd, fd_set &current_sockets)
{
	std::string server_message = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: 15\r\n\
Keep-Alive: timeout=5, max=1000\r\n\
\r\n\
1234567890\r\n\r\n";

	std::signal(SIGPIPE, SIG_IGN);
	if ((send(fd, server_message.c_str(), server_message.length(), 0)) < 0)
	{
		if (errno == EPIPE)
		{
			std::cout << "++connexion with client is lost, closing fd : " << fd << "++" << std::endl;
			FD_CLR(fd, &current_sockets);
			close(fd);
		}
	}
	else
		std::cout << "Message send to the client fd : " << fd << "++" << std::endl;
}
