#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP
#include "common.hpp"
#include "server.class.hpp"

/*
	Defining Macro for commande names
*/
# define CMD_END_WEBSERV "quit"
# define CMD_PAUSE "pause"
# define CMD_RUN "run"
# define CMD_RESTART_WEBSERV "restart"
# define CMD_PAUSE_SERV "pause server"
# define CMD_RUN_SERV "run serverv"
# define CMD_CHANGE_CONFIG_FILE "config"

/* 
	Defining Macro to acces the bit of the state info about webserv
*/
#define WS_ISRUNNING (_state & (1 << 0))
#define WS_ISINPAUSE (_state & (1 << 1))

/*
	Macro to set the bit of options 
*/
#define WS_SETRUN (_state = _state | (1 << 0))
#define WS_SETPAUSE (_state = _state | (1 << 1))
#define WS_SETUNPAUSE (_state = _state & ~(1 << 1))
#define WS_SETEND (_state = _state & ~(1 << 0))

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
		char					_state;

		bool	__is_a_socket(int fd);
		void	__console(fd_set &current_sockets);
};

#endif

