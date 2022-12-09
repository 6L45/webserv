#include "server.class.hpp"

Server::Server(__server_conf sc)
	:	_port(sc.port),
		_host(sc.host),
		_index(sc.index),
		_root(sc.root),
		_name(sc.server_name),
		_options(sc.options),
		_methods(sc.methods)
{ }

// Server::Server(int port)
// {
// 	this->_domain =		AF_INET | PF_INET;
// 	this->_service =	SOCK_STREAM;
// 	this->_protocol =	0;
// 	this->_port =		port;
// 	this->_interface =	INADDR_ANY;

// 	this->_address.sin_family =			this->_domain; //AF_INET
// 	this->_address.sin_port =			htons(this->_port);
// 	this->_address.sin_addr.s_addr =	htonl(this->_interface); // INADRR_ANY

// 	// socket creation 
// 	this->_sock = socket(this->_domain, this->_service, this->_protocol);
// 	fcntl(_sock, F_SETFL, O_NONBLOCK);
// 	// bind socket to a port
// 	if ( (bind(this->_sock, (struct sockaddr *)&(this->_address), 
// 					sizeof(this->_address))) < 0 )
// 	{
// 		perror("Error : bind socket");
// 		exit(EXIT_FAILURE);
// 	}

// 	// listen to the port
// 	if ( (listen(this->_sock, 10)) < 0 ) // <--- second arg = backlog : defines the maximum length to which the queue of pending connections for sockfd may grow
// 	{
// 		perror("Error : listen");
// 		exit(EXIT_FAILURE);
// 	}

// 	std::cout << "connected and listening : " << this->_sock << std::endl;
// }

bool		Server::belong_to(const std::string& host_port) const
{
	std::string s_host(host_port, 0, host_port.find(':'));
	std::string s_port(host_port, host_port.find(':') + 1); //no except safe

	if (s_port.compare(std::to_string(_port)))
		return (false);
	for (std::vector<std::string>::const_iterator c_it = _host.begin(); c_it != _host.end(); c_it++)
	{
		if (! (*c_it).compare(s_host))
			return (true);
	}

	return (false);
}

// void	Server::send_response(int fd, fd_set &current_sockets, std::string &response)
// {

// /*
// 	std::string server_message = "HTTP/1.1 404 Not Found\r\n\
// Server: nginx/0.8.54\r\n\
// Date: Mon, 02 Jan 2012 02:33:17 GMT\r\n\
// Content-Type: text/html\r\n\
// Content-Length: 169\r\n\
// Connection: keep-alive\r\n\
// Keep-Alive: timeout=5, max=1000\r\n\
// \r\n\
// <html>\r\n\
// <head><title>404 Not Found</title></head>\r\n\
// <body bgcolor=\"white\">\r\n\
// <center><h1>404 Not Found</h1></center>\r\n\
// <hr><center>nginx/0.8.54</center>\r\n\
// </body>\r\n\
// </html>\r\n\
// \r\n";
// */

// /*
// 	std::string server_message = "HTTP/1.1 200 OK\r\n\
// Server: nginx/0.8.54\r\n\
// Date: Mon, 02 Jan 2012 02:33:17 GMT\r\n\
// Content-Type: text/html\r\n\
// Content-Length: 161\r\n\
// Connection: keep-alive\r\n\
// Keep-Alive: timeout=5, max=1000\r\n\
// \r\n\
// <html>\r\n\
// <head><title>200 0K</title></head>\r\n\
// <body bgcolor=\"white\">\r\n\
// <center><h1>200 OK Found</h1></center>\r\n\
// <hr><center>nginx/0.8.54</center>\r\n\
// </body>\r\n\
// </html>\r\n\
// \r\n";
// */

// 	std::cout << "---------------- " << "response server : " << std::endl
// 	<< response << std::endl << "----------------" << std::endl << std::endl;

// 	std::signal(SIGPIPE, SIG_IGN);
// 	if ((send(fd, response.c_str(), response.length(), 0)) < 0)
// 	{
// 		if (errno == EPIPE)
// 		{
// 			std::cout << "++connexion with client is lost, closing fd : " << fd << "++" << std::endl;
// 			FD_CLR(fd, &current_sockets);
// 			close(fd);
// 		}
// 	}
// 	else
// 		std::cout << "Message send to the client fd : " << fd << "++" << std::endl;
// }
