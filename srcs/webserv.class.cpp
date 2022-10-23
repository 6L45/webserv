#include "webserv.class.hpp"

Webserv::Webserv()
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
	int j = 0;
	int	fd_max = 0;
	// chercher le plus grand FD ouvert pour la taille de reserve	
	for (std::vector<Server>::iterator it = tmp.begin(); it != tmp.end(); it++, i++)
	{
		if (tmp[i].get_socket() > fd_max)
			fd_max = tmp[i].get_socket();
	}
	this->_servs.reserve(fd_max + 1);
	
	i = 0;
	for (std::vector<Server>::iterator it = tmp.begin(); it != tmp.end(); it++, i++)
	{
		while (tmp[i].get_socket() != j)
			j++;
		this->_servs[j] = tmp[i];
		this->_server_fd.push_back(tmp[i].get_socket());
		j = 0;
	}

// IMPORTANT : bien s'assurer que tous les fd soient fermées avant d'arriver ici
// ou créer autant d'empty serv qu'il y'a de fd ouvert
// le fd socket des serveurs sert d'indice d'acces [] dans this->_servs pour acceder au serveur voulu
 
/*
	this->_servs.push_back(empty_serv);
	this->_servs.push_back(empty_serv);
	this->_servs.push_back(empty_serv);
	this->_servs.push_back(server1);
	this->_servs.push_back(server2);

	this->_server_fd.push_back(server1.get_socket());
	this->_server_fd.push_back(server2.get_socket());
*/
}

void 	Webserv::launch()
{
	struct sockaddr_in	client;
	socklen_t			client_len;
	fd_set				current_sockets, ready_sockets;

//	uint16_t			conn_port;
	
	FD_ZERO(&current_sockets);
	for (std::vector<int>::iterator it = this->_server_fd.begin(); it != this->_server_fd.end(); it++)
		FD_SET(this->_servs[*it].get_socket(), &current_sockets);
	 
	std::cout << "++waiting for connection ++" << std::endl; 

	while (1)
	{
		ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) // 1st param = size // 2nd = fd to check for reading // 3rd = fd to check for writing // 4th = error // 5th time
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
					std::cout << "++connexion request on socket fd : " << i << "++" << std::endl;
					
					this->_confd = accept(i, (SA *)NULL, NULL);
					if (this->_confd < 0)
					{
						// ICI renvoyer la bonne erreur http en cas de problème connexion
						std::cout << "connection error : serveur full or whatever\
créer un error handler ici sur le retour d'accept" << std::endl;
					}
					this->_fds.insert(std::pair<int, int>(this->_confd, i));
					FD_SET(this->_confd, &current_sockets);

					std::cout << "++Connexion accepted, client fd : " << this->_confd << " ++" << std::endl;
				}
				else
				{
					std::cout << "++client request on socket fd : " << i << " ++" << std::endl;
					
					// peut être update par une seule fonction du genre "request handler"
					this->_servs[this->_fds[i]].print_request_client(i);
					this->_servs[this->_fds[i]].send_response(i, current_sockets);
				}
			}
		}
	}
}

bool	Webserv::__is_a_socket(int fd)
{
	return (std::find(this->_server_fd.begin(), this->_server_fd.end(), fd)
			!= this->_server_fd.end());
}













// au cas ou j'en aurai besoin à un moment ou un autre
// permet de récupérer le port de connection de client

//					getsockname(fd, (SA *)&client, &client_len);
//					conn_port = ntohs(client.sin_port);

