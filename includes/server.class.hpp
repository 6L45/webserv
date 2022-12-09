#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

#include "common.hpp"
#include "conf.class.hpp"
#include "http_handler.hpp"

/* 
	Macro to access the bit of options 
		SV_DIRISACTIVE return true if directory browser is allowed, false otherwise
		SV_BODYISLIMITED return true if body required a max size, false otherwise
		SV_GETISSET, SV_PORTISSET , SV_DELETEISSET return true if the method is authorisez, false otherwise
*/
#define SV_DIRISACTIVE(server) (server._options & (1 << 0))
#define SV_BODYISLIMITED(server) (server._options & (1 << 1))
#define SV_GETISSET(server) (server._methods & (1 << 0))
#define SV_POSTISSET(server) (server._methods & (1 << 1))
#define SV_DELETEISSET(server) (server._methods & (1 << 2))

class Server
{
	public :
		Server(__server_conf sc);
		~Server() {};

		bool		belong_to(const std::string& host_port) const;
		// void		request_handler(int &fd);
		// void		send_response(int fd, fd_set &current_sockets, std::string &response);

	private :
		int							_port;
		std::vector<std::string>	_host;
		std::vector<std::string>	_index;
		std::string					_root;
		std::string					_name;
		char						_options;
		char						_methods;



		// struct	sockaddr_in		_address;
		// int						_sock;
		// int						_domain;
		// int						_service;
		// int						_protocol;
		// u_long					_interface;

		// char					_buff[MAXLINE + 1];
		// char					*_recvline;

	friend class Webserv;
	friend class Http_handler;
};

#endif