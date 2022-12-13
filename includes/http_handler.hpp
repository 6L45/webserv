#pragma once

# include "common.hpp"

class Server;

class Http_handler // : inherit from a execution class ? 
{
	private :
		std::string	header_http1		= "HTTP/1.1 ";
		std::string	header_content_len	= "Content-Length: ";
		std::string	header_content_loc	= "Content-Location: ";
		std::string	header_content_type	= "Content-Type: ";
		std::string	header_date 		= "Date: ";
		std::string	header_last_modif	= "Last-Modified: ";
		std::string	header_server		= "Server: FT_Webserv/1.0.0";
		std::string	header_encoding		= "Transfert-Encoding: identity";

	public :
	// DICO DECLARATION	
		typedef	std::multimap<std::string, std::string>					dico;
		typedef std::multimap<std::string, std::string>::iterator		MMAPIterator;
		typedef std::multimap<std::string, std::string>::const_iterator	MMAPConstIterator;

	// CONSTRUCT DESTRUCT	
		Http_handler(std::string &request); // receive htttp request
		virtual ~Http_handler() {};

	// GETTER
		std::string		get_host_name(void) const;
		std::pair<MMAPConstIterator, MMAPConstIterator>
						get_elems(const std::string key) const;

	// PUBLIC METHODS
		int				invalid_request(void) const;
		std::string		exec_request(Server &Serv);
		std::string		bad_request(void);

	private :
	//REQUEST RELATED
		std::string		_header;
		std::string		_response;
		dico			_req_dict;

	//SHORTCUTS
		int				_valid;
		std::string		_address;
		std::string		_method;

	// PRIVATE METHODS
		void			__directory_browser(const char *path, std::string const &host);
		std::string		__filesLst(std::string const &dirEntry, std::string const &dirName, std::string const &host);

		// method exec - response init 		
		void			__GET_response(std::string &value, Server &serv);
		void			__DELETE_response(std::string &value, Server &serv);
		int				__POST_response(std::string &value, Server &serv);

		// response generator		
		void			__200_response(int ret);
		void			__err_header(int ret);

		// utils
		std::string		__get_time(void);
		void			__init_response(std::string &value);
};

# include "server.class.hpp"
