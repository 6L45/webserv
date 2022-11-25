#pragma once

# include "common.hpp"

class Server;

class Http_handler // : inherit from a execution class ? 
{
	public :
	
		typedef	std::multimap<std::string, std::string>					dico;
		typedef std::multimap<std::string, std::string>::iterator		MMAPIterator;
		typedef std::multimap<std::string, std::string>::const_iterator	MMAPConstIterator;
	
		Http_handler(std::string &request); // receive htttp request
		virtual ~Http_handler() {};

		// GETTER
		std::string		get_host_name(void) const;
		std::pair<MMAPConstIterator, MMAPConstIterator>
						get_elems(const std::string key) const;

		std::string		exec_request(Server &Serv);

/*		dico			get_req_dict(void) const;
		dico			get_res_dict(void) const;
		int				get_return_val(void) const;

		// EXECUTION HTTP REQUEST
		void			exec(void);

		// HTTP RESPONSE 
		std::string		header_construct(void);
		std::string		body_construct(void);
*/
	private :
		std::string		_request;
		std::string		_response;
		dico			_req_dict;
		dico			_res_dict;
		int				_ret;
};

#include "server.class.hpp"
