#include "webserv.class.hpp"

Webserv::Webserv()
	:	_state(1)
{
	// Les servers seront à initialiser les serveurs en fonction du fichier conf
//	Server	empty_serv;
	Server	server1(PORT);
	Server	server2(SERVER_PORT);

	// stock provisoirement dans tmp;
	std::vector<Server> tmp;
	tmp.push_back(server1);
	tmp.push_back(server2);

	int i = 0;
	int	fd_max = 0;
	// chercher le plus grand FD ouvert pour la taille de reserve	
	for (std::vector<Server>::iterator it = tmp.begin(); it != tmp.end(); it++, i++)
	{
		if (tmp[i].get_socket() > fd_max)
			fd_max = tmp[i].get_socket();
	}
	this->_servs.reserve(fd_max + 1);
	
	i = 0;
	int j = 0;
	for (std::vector<Server>::iterator it = tmp.begin(); it != tmp.end(); it++, i++)
	{
		while (tmp[i].get_socket() != j)
			j++;
		this->_servs[j] = tmp[i];
		this->_server_fd.push_back(tmp[i].get_socket());
		j = 0;
	}
}

void 	Webserv::launch()
{
	struct sockaddr_in	client;
	socklen_t			client_len;
	fd_set				ready_sockets, wrinting_socket;
/*	struct timeval		tempo;

	tempo.tv_sec = 0;
	tempo.tv_usec = 1;
*/
//	uint16_t			conn_port;
	client_len = sizeof(client);
	
	FD_ZERO(&wrinting_socket);
	FD_ZERO(&current_sockets);
	for (std::vector<int>::iterator it = this->_server_fd.begin(); it != this->_server_fd.end(); it++)
		FD_SET(this->_servs[*it].get_socket(), &current_sockets);
	FD_SET(STDIN_FILENO, &current_sockets); 
	std::cout << "++waiting for connection ++" << std::endl; 

	while (WS_ISRUNNING)
	{
		if (WS_ISINPAUSE)
		{
			FD_ZERO(&ready_sockets);
			FD_SET(STDIN_FILENO, &ready_sockets);
		}
		else
			ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
		{
			perror("select error");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (this->__is_a_socket(i))
				{
					std::cout << "++connexion request on socket fd : " << i << " ++" << std::endl;
					this->_confd = accept(i, (SA *)&client, &client_len);
					if (this->_confd < 0)
						std::cout << "accept connection error" << std::endl;

					this->_fds.insert(std::pair<int, int>(this->_confd, i));
					FD_SET(this->_confd, &current_sockets);

					std::cout << "++Connexion accepted, client fd : " << this->_confd << " ++" << std::endl;
//					std::cout << "sin_port : " << ntohs(client.sin_port) << std::endl;
//					std::cout << "sin_addr : " << ntohs(client.sin_addr.s_addr) << std::endl;
				}
				else if (i == STDIN_FILENO)
					this->__console(current_sockets);
				else
				{
					std::cout << "++client request on socket fd : " << i << " ++" << std::endl;
					
					// peut être update par une seule fonction du genre "request handler"
					print_request_client(i);
					this->_servs[this->_fds[i]].send_response(i, current_sockets);
				}	
			}
		}
	}
}

void	Webserv::print_request_client(int fd)
{
	int	n;

	std::vector<char> buff(MAXLINE); // C++ version

	std::cout << std::endl;
	while ( (n = recv(fd, buff.data(), buff.size(), MSG_DONTWAIT)) > 0 );

	if (n < 0)
	{
		if (errno != EAGAIN)
		{
			perror("webserv read error");
			close(fd); //need to delete in fds in webserv the fd in map? to avoid select error..
			this->_fds.erase(fd);
			FD_CLR(fd, &current_sockets);
		}
	}

	std::string	request(buff.data());
	Http_handler request_handler(request);
}

bool	Webserv::__is_a_socket(int fd)
{
	return (std::find(this->_server_fd.begin(), this->_server_fd.end(), fd)
			!= this->_server_fd.end());
}

void	Webserv::__console(fd_set &current_sockets)
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
}











// au cas ou j'en aurai besoin à un moment ou un autre
// permet de récupérer le port de connection de client

//					getsockname(fd, (SA *)&client, &client_len);
//					conn_port = ntohs(client.sin_port);

