#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "common.hpp"

class Server
{
	public :
		Server(int port);
		Server() { this->_sock = -1; }; //empty serv
		~Server() {};

		void		request_handler(int &fd);
		int			&get_socket(void);
		void		print_request_client(int fd);
		void		send_response(int fd, fd_set &current_sockets);

	private :
		struct	sockaddr_in		_address;
		int						_sock;
		int						_domain;
		int						_service;
		int						_protocol;
		int						_port;
		u_long					_interface;

		char					_buff[MAXLINE + 1];
		char					*_recvline;
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
		int						_confd;
		std::vector<int>		_server_fd;
		std::map<int, int>		_fds;
		std::vector<Server>		_servs;

		bool	__is_a_socket(int fd);

};

#endif

