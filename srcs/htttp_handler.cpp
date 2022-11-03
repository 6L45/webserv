#include "http_handler.hpp" 

Http_handler::Http_handler(std::string request)
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
/*
	for (dico::iterator it = this->_req_dict.begin(); it != this->_req_dict.end(); it++)
	{
		std::cout << '[' << (*it).first << "] -> " << (*it).second << std::endl;
	}
*/
}
