#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP
#include "common.hpp"
#include "server.class.hpp"
#include "conf.class.hpp"
#include "Client.class.hpp"

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
		Webserv(Conf config);
		~Webserv();
		
		// public methods
		void 	launch();

	private :

		Webserv();
		// int						_confd;
		// std::vector<int>		_server_fd; //les sockets des serveurs?
		// std::map<int, int>		_fds;	// les connexions mappé sur le socket surveur
		// std::vector<Server>		_servs; // les serveurs
		// fd_set					current_sockets;


		typedef	std::vector<Server>::const_iterator	server_pointer;
		typedef int									port_fd;

		std::multimap<port_fd,server_pointer>		_port_host;
		std::map<int,port_fd>						_track; // link the connexion port with the opened fd
		std::vector<Server>							_servers; //vector car le nombre de serveur reste le même
		std::list<Client>							_clients; //list car beaucoup de connexion et deconnexions sont attendues.
		char										_state;
		fd_set										_current_sockets;

		Conf										_conf;

		bool		__is_a_socket(int fd) const;
		void		__console();
		void		__close_connexion(int fd);
		void		__print_connexions_stats() const;
		port_fd		__create_socket(int port);
		int			__get_the_port(int fd);
		std::string	__http_process(int fd, std::string &request);

		void	request_handler(int fd); // webserv recupere la requete du client pour la donner au serveur qui garde en interne la manip a faire avec http selon ses valeurs
};

#endif

