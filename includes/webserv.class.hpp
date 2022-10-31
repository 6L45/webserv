#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP
#include "common.hpp"
#include "server.class.hpp"

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
		void	__console(fd_set &current_sockets);
};

#endif

