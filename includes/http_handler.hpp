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
		std::string	header_connect		= "Connection: ";
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
		void			__CGI_exec(const std::string path);
		void			__GET_response(std::string &value, Server &serv);
		void			__DELETE_response(std::string &value, Server &serv);
		int				__POST_response(std::string &value, Server &serv);

		// response generator		
		void			__200_response(int ret);
		void			__err_header(int ret);
		void			__body_gen(int ret);

		// utils
		std::string		__get_time(void);
		void			__clean_address(void);
		void			__close_and_throw(std::fstream &file, int err);

};

typedef struct links
{
	std::string	trololol	= "		<center><iframe src=\"https://giphy.com/embed/Rkis28kMJd1aE\" width=\"480\" height=\"317\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	haha		= "		<center><iframe src=\"https://giphy.com/embed/uprwwjptZW4Za\" width=\"480\" height=\"358\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	hihi		= "		<center><iframe src=\"https://giphy.com/embed/9ZOyRdXL7ZVKg\" width=\"480\" height=\"360\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	hoho		= "		<center><iframe src=\"https://giphy.com/embed/ReImZejkBnqYU\" width=\"480\" height=\"270\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	vmars		= "		<center><iframe src=\"https://giphy.com/embed/R195TgCcRZNZe\" width=\"480\" height=\"271\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe><p></center>";
	std::string	nope		= "		<center><iframe src=\"https://giphy.com/embed/1iTI7wQfPEJu1wjK\" width=\"480\" height=\"271\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	kpnope		= "		<center><iframe src=\"https://giphy.com/embed/WVUWw9ItZS048\" width=\"480\" height=\"305\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	monica		= "		<center><iframe src=\"https://giphy.com/embed/XymaJlgorUL8vOfF88\" width=\"480\" height=\"400\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	riley		= "		<center><iframe src=\"https://giphy.com/embed/mGbjatyEFVxdDRnQ9U\" width=\"480\" height=\"270\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	noLno		= "		<center><iframe src=\"https://giphy.com/embed/spfi6nabVuq5y\" width=\"480\" height=\"392\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	hmhmhm		= "		<center><iframe src=\"https://giphy.com/embed/15aGGXfSlat2dP6ohs\" width=\"480\" height=\"272\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	jesus		= "		<center><iframe src=\"https://giphy.com/embed/kd9h44W2CpZBLH7QpA\" width=\"360\" height=\"480\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	pickleR		= "		<center><iframe src=\"https://giphy.com/embed/SANT0ESrslle7yE9xV\" width=\"480\" height=\"270\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	not2D		= "		<center><iframe src=\"https://giphy.com/embed/RGSZZQC7SSc91VhDe7\" width=\"480\" height=\"270\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	RickFk		= "		<center><iframe src=\"https://giphy.com/embed/fvfRNXU8gkH0sLvDzQ\" width=\"480\" height=\"270\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	std::string	RickSht		= "		<center><iframe src=\"https://giphy.com/embed/3Fdskc7J0timI\" width=\"480\" height=\"270\" frameBorder=\"0\" class=\"giphy-embed\" allowFullScreen></iframe></center>";
	
	std::vector<std::string>	hehe;

	links()
	{
		hehe.push_back(trololol);
		hehe.push_back(haha);
		hehe.push_back(hihi);
		hehe.push_back(hoho);
		hehe.push_back(vmars);
		hehe.push_back(nope);
		hehe.push_back(kpnope);
		hehe.push_back(monica);
		hehe.push_back(riley);
		hehe.push_back(noLno);
		hehe.push_back(hmhmhm);
		hehe.push_back(jesus);
		hehe.push_back(pickleR);
		hehe.push_back(not2D);
		hehe.push_back(RickFk);
		hehe.push_back(RickSht);
		
		hehe.shrink_to_fit();
	}

	std::string	operator[](int n)
	{
		if (n)
			return ( hehe[std::rand() % (hehe.size())] );
		return ( hehe[std::rand() % (hehe.size())] );
	}

} memelord;

# include "server.class.hpp"
