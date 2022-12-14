#include "webserv.class.hpp"

Webserv::Webserv(Conf config, char** env)
	:	_state(1),
		_conf(config),
		_max_connexion(config._max_connexion),
		_env(env)
{
	port_fd					socket; // opened socket
	
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		t_keep_alive			deflt;
		deflt.fileno_server = true;
		deflt.fd = -1;
		this->_timer.push_back(deflt);
	}

	FD_ZERO(&_current_sockets);

	for (std::vector<__server_conf>::const_iterator it = _conf._sc.begin(); it != _conf._sc.end(); it++)
	{
		Server new_server(*it, env);
		_servers.push_back(new_server);
	}
	for (std::vector<Server>::const_iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		std::map<int,port_fd>::const_iterator it_track = _track.find(it->_port);
		/*
			if ()
				the socket on the listener port of this server is already opened (by a previous server using the same port)
				Adding only port-server in multimap
			else ()
				we create the new socket on the selected port and adding it on the map track, the fd_set, and in multimap port-server
		*/
		if (it_track != _track.end())
			_port_host.insert(std::make_pair(it_track->second, it));
		else
		{
			socket = __create_socket(it->_port);
			_track.insert(std::make_pair(it->_port, socket));
			_port_host.insert(std::make_pair(socket, it));
			FD_SET(socket, &_current_sockets);
		}
	}
}

Webserv::port_fd	Webserv::__create_socket(int port)
{
	struct	sockaddr_in	address;
	port_fd				new_socket;

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	// socket creation 
	new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (new_socket == -1)
	{
		std::cerr << "-- Socket error : unable to create the socket -> port " << port << " is not connected" << std::endl;
		return (-1);
	}
	// set the socket to a non blocking state
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "-- Fcntl error : unable to make the socket non blocking -> port " << port << " is not connected" << std::endl;
		close (new_socket);
		return (-1);
	}
	// bind the socket to accept connexions
	if ( (bind(new_socket, (struct sockaddr *)&(address), sizeof(address))) == -1 )
	{
		std::cerr << "-- bind error : unable to bind the socket to the port -> port " << port << " is not connected" << std::endl;
		close (new_socket);
		return (-1);
	}
	// listen to the port
	if ( (listen(new_socket, 30)) == -1 ) // <--- second arg = backlog : defines the maximum length to which the queue of pending connections for sockfd may grow
	{
		std::cerr << "-- listen error : unable to listen on the socket -> port " << port << " is not connected" << std::endl;
		close (new_socket);
		return (-1);
	}
	return (new_socket);
}

/* Unauthorized constructor */
Webserv::Webserv()
{ }

Webserv::~Webserv()
{
	for (std::list<Client>::const_iterator c_it = _clients.begin(); c_it != _clients.end(); c_it++)
		close(c_it->_fd);
	_clients.clear(); // NOT needed
	for (std::map<int,port_fd>::const_iterator c_it = _track.begin(); c_it != _track.end(); c_it++)
		close(c_it->second);
	FD_ZERO(&_current_sockets); // just in case
	std::cout << "++ closing the webserver ++" << std::endl;
	std::cout << "If you enjoyed the experience, please donate on Paypal to @webservteam42" << std::endl;
}

void 	Webserv::launch()
{
	struct sockaddr_in	client;
	socklen_t			client_len;
	fd_set				ready_sockets;
	port_fd				fd;
	int					select_ret;
	int					write = -1;
	struct timeval		tempo;


	tempo.tv_sec = 0;
	tempo.tv_usec = 1;
	client_len = sizeof(client);

	/* Listening to STDIN for console manipulations */
	FD_SET(STDIN_FILENO, &_current_sockets);

	while (WS_ISRUNNING)
	{
		if (write == -1)
			std::cout << "++ waiting for connections ++" << std::endl; 
		if (WS_ISINPAUSE)
		{
			FD_ZERO(&ready_sockets);
			FD_SET(STDIN_FILENO, &ready_sockets);
		}
		else
			ready_sockets = _current_sockets;

		select_ret = select(FD_SETSIZE, &ready_sockets, NULL, NULL, &tempo);
		if (select_ret < 0)
		{
			perror("fatal error : select");
			std::cout << errno << std::endl;
			break ;
		}
		if (select_ret == 0)
		{
			for (std::vector<t_keep_alive>::iterator it = this->_timer.begin();
					it != this->_timer.end(); it++)
			{
				if (!it->fileno_server && it->fd > 0)
				{
					clock_t	now = std::clock();
					if (((now - it->t) / CLOCKS_PER_SEC) > it->keep_alive)
					{
						send_response(it->fd, Http_handler::http_408());
						__close_connexion(it->fd);
					}
				}
			}

			write = (write + 1) % 5;
			std::cout << '\r';
			for (int i = 0; i < 5; i++)
			{
				if (i < write)
					std::cout << '=';
				else
					std::cout << ' ';
			}
			std::cout << '>';
			std::cout.flush();
			continue ;
		}
		write = -1;
		std::cout << std::endl;
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (this->__is_a_socket(i))
				{
					std::cout << "++ Connexion request on socket port : " << __get_the_port(i) << " ++" << std::endl;
					fd = accept(i, (SA *)&client, &client_len);
					if (fd == -1)
					{
						std::cerr << "-- accept error : failed to accept the connexion on the port " << __get_the_port(i) << std::endl;
						continue ;
					}
					if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
					{
						std::cerr << "-- Fcntl error : unable to make the client file descriptor non blocking -> connexion non accepted " << std::endl;
						close (fd);
						continue ;
					}
					_clients.push_back(Client(fd,__get_the_port(i),0));
					FD_SET(fd, &_current_sockets);
					std::cout << "++ Connexion accepted ++" << std::endl;

					this->_timer[fd].fd = fd;
					this->_timer[fd].fileno_server = false;
					this->_timer[fd].t = std::clock();
					this->_timer[fd].keep_alive = KEEP_ALIVE;
				}
				else if (i == STDIN_FILENO)
					this->__console();
				else
				{
					std::cout << "++ client request on port : " << __get_the_port(i) << " ++" << std::endl;
					request_handler(i);
				}	
			}
		}
	}
}

void	Webserv::__close_connexion(const int fd)
{
	close(fd);
	for (std::list<Client>::const_iterator c_it = _clients.begin(); c_it != _clients.end(); c_it++)
	{
		if (c_it->_fd == fd)
		{
			_clients.erase(c_it);
			break ;
		}
	}
	FD_CLR(fd, &_current_sockets);
	this->_timer[fd].fd = -1;
	std::cout << "++ connexion closed on fd : " << fd << " ++" << std::endl;
}

int		Webserv::__get_the_port(int fd)
{
	for (std::map<int,port_fd>::const_iterator c_it = _track.begin(); c_it != _track.end(); c_it++)
	{
		if (c_it->second == fd)
			return (c_it->first);
	}
	for (std::list<Client>::const_iterator c_it = _clients.begin(); c_it != _clients.end(); c_it++)
	{
		if (c_it->_fd == fd)
			 return (c_it->_port);
	}
	return (-1);
}

void	Webserv::request_handler(int fd)
{
	int	n;

	std::vector<char> buff(MAXLINE); // C++ version

	std::cout << std::endl;
	//while ( > 0);
	n = recv(fd, buff.data(), buff.size(), 0);
	if (n == 0)
	{
		std::cout << "++ Connexion is closed on the client side ++" << std::endl;
		return (__close_connexion(fd));
	}
	else if (n < 0)
	{
		if (errno != EAGAIN)
		{
			std::cerr << "-- recv error : something went wrong when recieving message -> client disconnected " << std::endl;
			return (__close_connexion(fd));
		}
	}
	std::string	request(buff.data());
	request.erase(std::remove(request.begin(), request.end(), '\r'), request.end());
	__http_process(fd, request);
}

bool	Webserv::__is_a_socket(int fd) const
{
	/* 
		A file descriptor is an opened socket on listening port is 
		the key argument in multimap port-host
	*/
	return (_port_host.find(fd) != _port_host.end());
}

void	Webserv::__console()
{
	std::string cmd;
	std::string cmd_to_compare;

	std::getline(std::cin, cmd);
	if (!cmd.compare(0, (cmd_to_compare = CMD_END_WEBSERV).size(), CMD_END_WEBSERV))
		WS_SETEND;
	else if (!cmd.compare(0, (cmd_to_compare = CMD_PAUSE).size(), CMD_PAUSE))
		WS_SETPAUSE;
	else if (!cmd.compare(0, (cmd_to_compare = CMD_RUN).size(), CMD_RUN))
		WS_SETUNPAUSE;
	else if (!cmd.compare(0, 4, "stat"))
		__print_connexions_stats();
}


void	Webserv::__print_connexions_stats() const
{
	std::cout << "Sockets (port,fd) : ";
	for (std::map<int,port_fd>::const_iterator c_it = _track.begin(); c_it != _track.end(); c_it++)
		std::cout << "( " << c_it->first << " , " << c_it->second << " ) ";
	std::cout << std::endl;
	std::cout << "Clients connected (port,fd) : ";
	for (std::list<Client>::const_iterator c_it = _clients.begin(); c_it != _clients.end(); c_it++)
		std::cout << "( " << c_it->_port << " , " << c_it->_fd << " ) ";
	std::cout << std::endl;
}

void	Webserv::__http_process(int fd, std::string &request)
{
	Http_handler	request_handler(request);
	std::string		response;

	if (request_handler.invalid_request())
	{
/*		if (invalid_request == 1 && !this->_timer[fd].body.empty())
		{
			if (== 0)
				request = this->_timer[fd].body;
				continue
			concat;
			return;
		}
*/		std::cout << "bad request 400 no host || 505 != HTTP/1.1" << std::endl;
		response = request_handler.bad_request();
		send_response(fd, response);
		__close_connexion(fd);
		return;
	}
	
	std::string	host_port = request_handler.get_host_name();
	std::vector<Server>::iterator it;
	for (it = _servers.begin(); it != _servers.end(); it++)
	{
		if ((*it).belong_to(host_port))
			break ;
	}
	if (it == _servers.end())
	{
		std::cout << "Host doesnt exist -> " << host_port << std::endl;
		__close_connexion(fd);
	}
	else
	{
		std::cout << "Host:Port target -> " << host_port << std::endl;
		std::cout << "Server name : " << it->_name << std::endl;
/*		if (request_handler.chunked())
		{
			std::string chunk = htpp_handler.chunkedhandler();
			this->_timer[fd].body += chunk; 
			if (chunked != 0)
				return;
			response = this->_timer[fd].body;
		}
		else
*/			response = request_handler.exec_request(*it);
		send_response(fd, response);
		if (request_handler.get_connection().compare("null"))
		{
			if ((!request_handler.get_connection().compare("close"))
					|| !request_handler.get_connection().compare("Close"))
				__close_connexion(fd);
			else if ((!request_handler.get_connection().compare("keep-alive"))
					|| !request_handler.get_connection().compare("Keep-Alive"))
			{
				this->_timer[fd].keep_alive = 5; // it->keep_alive
				this->_timer[fd].t = std::clock();
			}
		}
		if (request_handler.get_keep_alive())
		{
			this->_timer[fd].keep_alive = request_handler.keep_alive_value();
/*
			this->_timer[fd].keep_alive = std::min(this->_timer[fd], DEFAULT_KEEP_ALIVE)
*/
			this->_timer[fd].t = std::clock();
		}
	}
}

/*
	Sending the response to the client on fd
	//note, fd change to Client class
*/
void	Webserv::send_response(int fd, const std::string& response)
{
	std::cout << "---------------- " << "response server : " << std::endl
	<< response << "----------------" << std::endl << std::endl;
	int	ret;
	std::signal(SIGPIPE, SIG_IGN);
	if ((ret = send(fd, response.c_str(), response.length(), 0)) < 0)
	{
		if (errno == EPIPE)
		{
			std::cout << "++connexion with client is lost++" << std::endl;
			return (__close_connexion(fd));
		}
	}
	std::cout << "Message send to the client on port : " << __get_the_port(fd) << "++" << std::endl;
	this->_timer[fd].t = std::clock();
}






// au cas ou j'en aurai besoin ?? un moment ou un autre
// permet de r??cup??rer le port de connection de client

//					getsockname(fd, (SA *)&client, &client_len);
//					conn_port = ntohs(client.sin_port);

