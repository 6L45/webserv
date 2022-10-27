#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP
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
		in_addr_t				_interface;

		char					_buff[MAXLINE + 1];
		char					*_recvline;
};

#endif