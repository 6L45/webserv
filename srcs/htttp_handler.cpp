#include "http_handler.hpp" 

Http_handler::Http_handler(std::string &request)
{
	std::stringstream	req(request.c_str());
	std::string			line;

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
			index = line.find(';');
			if (line[0] == ' ')
				line = line.substr(1);
			val = line.substr(0, index);

			this->_req_dict.insert(std::make_pair(key, val));
		}
	}
}

std::string	Http_handler::exec_request(Server &serv)
{
	std::cout << std::endl << std::endl;
	std::cout << "++ ici l'execution ++" << std::endl;
	std::cout << std::endl;

	return ("No response yet");
}

std::string	Http_handler::get_host_name() const
	{ return (this->_req_dict.find("Host")->second); }


std::pair<typename Http_handler::MMAPConstIterator,
			typename Http_handler::MMAPConstIterator>
	Http_handler::get_elems(const std::string key) const
{
	return (this->_req_dict.equal_range(key));
}
