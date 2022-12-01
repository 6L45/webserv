#include "http_handler.hpp" 

Http_handler::Http_handler(std::string &request)
{
	std::stringstream	req(request.c_str());
	std::string			line;

	this->_response = "no reponse yet";

	std::cout << request << std::endl;
	while (std::getline(req, line, '\n'))
	{
		int			index;
		std::string	key;
		
		index = line.find(':');
		if (index == -1)
			index = line.find(' ');
		key = line.substr(0, index);

		while (index > 0)
		{
			std::string		val;
			line = line.substr(index + 1);
			if (line[0] == ' ')
				line = line.substr(1);
			index = line.find(';');
			val = line.substr(0, index);

			this->_req_dict.insert(std::make_pair(key, val));
		}
	}
}

std::string	Http_handler::exec_request(Server &serv)
{
	typename Http_handler::MMAPIterator	val;

	// TODO try execute catch exeption switch case
//	try
//	{
		if ( (val = this->_req_dict.find("GET")) != this->_req_dict.end() )
			this->__GET_method(val->second);
		else if ( (val = this->_req_dict.find("POST")) != this->_req_dict.end() )
			this->__POST_method(val->second);
		else if ( (val = this->_req_dict.find("DELETE")) != this->_req_dict.end() )
			this->__DELETE_method(val->second);
//		else
//			throw 400 bad request
/*	}
	catch (const std::exception& e) <= a modif pour recup int par throw
	{
		switch (e)
		{
			case 404
				__build_err_response(404); break;
			case XXX
				__build_err_response(XXX); break;
		}
		return (this->_header);
	}
*/
	std::cout << std::endl << std::endl;
	std::cout << "++ ici l'execution ++" << std::endl;
	std::cout << std::endl;

	return (this->_response);
}

std::string	Http_handler::get_host_name() const
	{ return (this->_req_dict.find("Host")->second); }


std::pair<typename Http_handler::MMAPConstIterator,
			typename Http_handler::MMAPConstIterator>
	Http_handler::get_elems(const std::string key) const
{
	return (this->_req_dict.equal_range(key));
}


void	Http_handler::__GET_method(std::string &value)
{
	std::string address = this->__check_address(value);
	std::string tmp = "./www";

	std::fstream	file;

	for (const auto &entry : std::filesystem::directory_iterator(tmp))
		std::cout << entry << std::endl;
	
	file.close();
}

void	Http_handler::__POST_method(std::string &value)
{

	std::string address = this->__check_address(value);
	// POST_exec(address);
}

void	Http_handler::__DELETE_method(std::string &value)
{

	std::string address = this->__check_address(value);
	// DELETE_exec(address);
}

std::string	Http_handler::__check_address(std::string &value)
{
	std::string	address;

	int index = value.find("HTTP");
	address = value.substr(0, index - 1);

//	if (address invalid)
//		throw 404 Not Found

	return (address);
}
