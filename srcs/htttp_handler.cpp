#include "http_handler.hpp" 
/*
Http_handler::Http_handler(std::string request)
{
	std::cout << request << std::endl;
}
*/

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

		std::cout << '[' << key << ']' << std::endl;	
		while (index > 0)
		{
			std::string		val;
			line = line.substr(index + 1);
			index = line.find(';');
			val = line.substr(0, index);

			std::cout << ">>> " << val << std::endl;	
		
		}
	}
}
