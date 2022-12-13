#pragma once

# include "common.hpp"

class Server;

class Http_handler // : inherit from a execution class ? 
{
	private :
		std::string	header_http1 = "HTTP/1.1 ";
		std::string	header_content_len = "Content-Length: ";
		std::string	header_content_loc = "Content-Location: ";
		std::string	header_content_type = "Content-Type: ";
		std::string	header_date = "Date: ";
		std::string	header_last_modif = "Last-Modified: ";
		std::string	header_server = "Server: FT_Webserv/1.0.0";
		std::string	header_encoding = "Transfert-Encoding: identity";

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
		void			directory_browser(const char *path, std::string const &host);
		std::string		filesLst(std::string const &dirEntry, std::string const &dirName, std::string const &host);
		int				invalid_request(void) const;
		std::string		bad_request(void);
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
		std::string		_address;
		std::string		_header;
		std::string		_response;
		std::string		_method;
		dico			_req_dict;
		int				_valid;
		int				_ret;

		void			__GET_method(std::string &value, Server &serv);
		void			__POST_method(std::string &value, Server &serv);
		void			__DELETE_method(std::string &value, Server &serv);
		void			__GET_response(std::string &value, Server &serv);
		int				__POST_response(std::string &value, Server &serv);
		void			__DELETE_response(std::string &value, Server &serv);
		void			__err_header(int ret);
		void			__200_response(int ret);
		std::string		__get_time(void);
		void			__init_response(std::string &value);
};

# include "server.class.hpp"
