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
		void			__body_gen(int ret);

		// utils
		std::string		__get_time(void);
		void			__init_response(std::string &value);

/*
*/

};

typedef struct links
{

	std::string	trololol	= "		<center><iframe src=\"https://giphy.com/embed/Rkis28kMJd1aE\" width=\"480\" height=\"317\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	lolilol		= "		<center><iframe src=\"https://giphy.com/embed/hLwSzlKN8Fi6I\" width=\"480\" height=\"356\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	haha		= "		<center><iframe src=\"https://giphy.com/embed/uprwwjptZW4Za\" width=\"480\" height=\"358\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	hihi		= "		<center><iframe src=\"https://giphy.com/embed/9ZOyRdXL7ZVKg\" width=\"480\" height=\"360\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	hoho		= "		<center><iframe src=\"https://giphy.com/embed/ReImZejkBnqYU\" width=\"480\" height=\"270\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";

	std::vector<std::string>	hehe;

	links()
	{
		hehe.push_back(trololol);
		hehe.push_back(lolilol);
		hehe.push_back(haha);
		hehe.push_back(hihi);
		hehe.push_back(hoho);
		
		hehe.shrink_to_fit();
	}

	std::string	operator[](int n)
	{
		if (n)
			return ( hehe[std::rand() % (hehe.size())] );
		return ( hehe[4] );
	}

} memelords;

# include "server.class.hpp"
