#ifndef WEBSERV_HPP
# define WEBSERV_HPP
#include "common.hpp"

class Webserv
{
	public :
		// constru - destru
		Webserv();
		~Webserv(){};
		
		// public methods
		void 	launch();

	private :
		struct	sockaddr_in	_address;
		int					_sock;
		int					_confd;
		int					_domain;
		int					_service;
		int					_protocol;
		int					_port;
		u_long				_interface;

		char				_buff[MAXLINE + 1];
		char				*_recvline;
  
		// private methods
		void	__print_request_client();
};

#endif

