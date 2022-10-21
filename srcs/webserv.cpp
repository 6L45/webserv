#include "webserv.hpp"

#define DEFAULT_MAX_CONNECT 30

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

//	uint16_t			conn_port;


/*	Adding servers sockets in the structure set for non blocking listeners
	Epoll for linux users
	Select for others
 */
#ifdef __linux__
	int					_epoll_sckt;
	struct epoll_event	_epoll_sckt_ev;
	struct epoll_event	_epoll_wait_ev[DEFAULT_MAX_CONNECT];
	int					_epoll_nfds;

	_epoll_sckt = epoll_create(DEFAULT_MAX_CONNECT);
	for (typename std::vector<int>::iterator it = this->_server_fd.begin(); it != this->_server_fd.end(); it++)
	{
		_epoll_sckt_ev.events = EPOLLIN;
		_epoll_sckt_ev.data.fd = this->_servs[*it].get_socket();
		if (epoll_ctl(_epoll_sckt, EPOLL_CTL_ADD, this->_servs[*it].get_socket(), &_epoll_sckt_ev) == -1)
			perror("epoll_ctl: unable to add server socket");
	}
#else
	fd_set				current_sockets, ready_sockets;
	FD_ZERO(&current_sockets);
	for (typename std::vector<int>::iterator it = this->_server_fd.begin(); it != this->_server_fd.end(); it++)
		FD_SET(this->_servs[*it].get_socket(), &current_sockets);
#endif

	std::cout << "++waiting for connection ++" << std::endl; 
	while (1)
	{

/* Non blocking waiting for event
	Linux users - maxevents under 1s
	Other - blocking until event
 */
#ifdef __linux__
		_epoll_nfds = epoll_wait(_epoll_sckt, _epoll_wait_ev, DEFAULT_MAX_CONNECT, 1000);
		if (_epoll_nfds == -1)
		{
			perror("epoll_wait error");
			exit(EXIT_FAILURE); //ici quitter est un peu drastique j'ai l'impression
		}
#else
		ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) // 1st param = size // 2nd = fd to check for reading // 3rd = fd to check for writing // 4th = error // 5th time
		{
			perror("select error");
			exit(EXIT_FAILURE);
		}
#endif
/* For loops checking the triggered fd (socket or clients) 
	Linux - structure contains all data of fd which is set
	Other - need to check all possible fd and test if is set
*/
#ifdef __linux__
		for (int i = 0; i < _epoll_nfds; i++)
		{
			if ((_epoll_wait_ev[i].events & EPOLLIN) == EPOLLIN)
			{
				if (this->__is_a_socket(_epoll_wait_ev[i].data.fd))
				{
					std::cout << "++connexion request on socket fd : " << i << "++" << std::endl;
					
					this->_confd = accept(_epoll_wait_ev[i].data.fd, (SA *)NULL, NULL);
					if (this->_confd < 0)
					{
						// ICI renvoyer la bonne erreur http en cas de problème connexion
						std::cout << "connection error : serveur full or whatever\
créer un error handler ici sur le retour d'accept" << std::endl;
					}
					_epoll_sckt_ev.events = EPOLLIN;
					_epoll_sckt_ev.data.fd = this->_confd;
					if (epoll_ctl(_epoll_sckt, EPOLL_CTL_ADD, this->_confd, &_epoll_sckt_ev) == -1)
					{
						perror("epoll_ctl: unable to add the client socket");
						close(this->confd);
					}
					else
					{
						this->_fds.insert(std::pair<int, int>(this->_confd, i));
						std::cout << "++Connexion accepted, client fd : " << this->_confd << " ++" << std::endl;
					}
				}
				else
				{
					std::cout << "++client request on socket fd : " << i << " ++" << std::endl;
					
					// peut être update par une seule fonction du genre "request handler"
					this->_servs[this->_fds[_epoll_wait_ev[i].data.fd]].print_request_client(_epoll_wait_ev[i].data.fd);
					this->_servs[this->_fds[_epoll_wait_ev[i].data.fd]].send_response(_epoll_wait_ev[i].data.fd, _epoll_sckt);
				}
			}
#else
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
#endif
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

