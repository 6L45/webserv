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
#define SV_DIRISACTIVE(location) (location._options & (1 << 0))
#define SV_BODYISLIMITED(location) (location._options & (1 << 1))
#define SV_GETISSET(location) (location._methods & (1 << 0))
#define SV_POSTISSET(location) (location._methods & (1 << 1))
#define SV_DELETEISSET(location) (location._methods & (1 << 2))

class Server
{
	public :
		Server(__server_conf sc, char **env);
		~Server() {};

		bool		belong_to(const std::string& host_port) const;
		// bool		cgi_exec(const std::string& file_name) const;
		// std::string		get_cgi_path(const std::string& file_name) const;

	private :
		typedef __location			location;
		int							_port;
		std::vector<std::string>	_host;
		std::vector<std::string>	_index;
		std::string					_root;
		std::string					_name;
		char						**_env;
		std::vector<location>		_location;

	friend class Webserv;
	friend class Http_handler;
};

#endif