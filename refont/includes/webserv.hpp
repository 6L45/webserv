#ifndef WEBSERV_HPP
# define WEBSERV_HPP
#include "common.hpp"

class Server
{
	public :
		Server(int port) {};
		~Server() {};

	private :

};

class Webserv
{
	public :
		// constru - destru
		Webserv();
		~Webserv(){};
		
		// public methods
		void 	launch();

	private :
	//------------------------------------------------------
		struct	sockaddr_in		_address;	//|
		struct	sockaddr_in		_address2;	//|__ pas besoin de 2 de chaque dans serv
		int						_sock;		//|    c'etait pour tests sur plusieurs sockets dans Webserv
		int						_sock2;		//|
		int						_domain;
		int						_service;
		int						_protocol;
		int						_port;
		u_long					_interface;

		char					_buff[MAXLINE + 1];
		char					*_recvline;
	//------------------------------------------------------- à passer dans Server

		int						_confd;
		std::list<int>			_server_fd; // list des fd à surveiller pour le accept
		std::map<int, int>		_fds;		// <- fd connexion (this->_connfd) // second fd server correspondant
		std::vector<Server>		_servs;		// prévoir 0 1 2 vide pour appeler les server directement par leur fd
											// exemple : this->_servs[map[ntohs(client.sin_port)].second].method()

		// private methods
		void	__print_request_client(void); // a passer dans Server (handle_request) avec valeur de retour (HTTP protocole)
		void	__bind_and_listen(void); // Server construc (+ ou -)
};

#endif

