#pragma once

# include "common.hpp"

class Http_handler // : inherit from a execution class ? 
{
	typedef	std::multimap<std::string, std::string>	dico;

	public :
		Http_handler(std::string request); // receive htttp request
		Http_handler(int retvalue) {}; // receive valeur de retour
		virtual ~Http_handler() {};
/*
		// GETTER
		dico			get_req_dict(void) const;
		dico			get_res_dict(void) const;
		int				get_return_val(void) const;

		// EXECUTION HTTP REQUEST
		void			exec(void); // <- to remove if inherit ? 

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
