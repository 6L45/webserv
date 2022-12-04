#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

#include "common.hpp"
#include "conf.class.hpp"
#include "http_handler.hpp"

class Server
{
	public :
		Server(int port);
		Server(__server_conf sc);
		Server() { this->_sock = -1; }; //empty serv
		~Server() {};

		bool		belong_to(const std::string& host_port) const;
		void		request_handler(int &fd);
		int			&get_socket(void);
		void		print_request_client(int fd);
		void		send_response(int fd, fd_set &current_sockets, std::string &response);

	private :
		int							_port;
		std::vector<std::string>	_host;
		std::vector<std::string>	_index;
		std::string					_root;
		std::string					_name;



		struct	sockaddr_in		_address;
		int						_sock;
		int						_domain;
		int						_service;
		int						_protocol;
		u_long					_interface;

		char					_buff[MAXLINE + 1];
		char					*_recvline;

	friend class Webserv;
	friend class Http_handler;
};

#endif