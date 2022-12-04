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
	try
	{
		if ( (val = this->_req_dict.find("GET")) != this->_req_dict.end() )
			this->__GET_method(val->second, serv);
		else if ( (val = this->_req_dict.find("POST")) != this->_req_dict.end() )
			this->__POST_method(val->second, serv);
		else if ( (val = this->_req_dict.find("DELETE")) != this->_req_dict.end() )
			this->__DELETE_method(val->second, serv);
//		else
//			throw 400 bad request
	}
	catch (int e) // <= a modif pour recup int par throw
	{
		switch (e)
		{
			case 404 :
				__err_header(404); break;
/*			case XXX
				__build_err_response(XXX); break;
*/		}
		return (this->_response);
	}

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


void	Http_handler::__GET_method(std::string &value, Server &serv)
{
	this->__check_address(value, serv);

	
	this->header_http1 += "200 OK\r\n";
	this->header_content_len += std::to_string(this->_response.length() - 4) + "\r\n";
	this->header_content_loc += this->_address + "\r\n";
	this->header_content_type += "text\r\n";
	this->header_date += this->__get_time() + "\r\n";
	
	this->_header += this->header_http1 + this->header_content_len
				+ this->header_content_loc + this->header_content_type
				+ this->header_date + this->header_server + "\r\n"
				+ this->header_encoding + "\r\n"
				+ "\r\n";
	
	this->_response = this->_header + "\r\n" + this->_response + "\r\n\r\n";
}

void	Http_handler::__POST_method(std::string &value, Server &serv)
{

	this->__check_address(value, serv);
	// POST_exec(address);
}

void	Http_handler::__DELETE_method(std::string &value, Server &serv)
{

	this->__check_address(value, serv);
	// DELETE_exec(address);
}

void	Http_handler::__check_address(std::string &value, Server &serv)
{
	int index = value.find("HTTP");
	this->_address = value.substr(0, index - 1);

	// adresse juste apres le GET
	std::string	request_loc(this->_address);
	
	// Si l'adresse est home -> '/'  look dans config files les index
	if  (!request_loc.compare("/"))
	{
		std::cout << "------------> "<< serv._index.size() << std::endl;
		// parcours les index
		for (std::vector<std::string>::iterator it = serv._index.begin();
				it != serv._index.end(); it++)
		{
			std::cout << "---------------------> " << serv._root + *it << std::endl;
			std::ifstream	file;
			// teste ouverture des index
			file.open(serv._root + *it, std::ios::in);
			if (file.is_open())
			{
				// read / get file and close file
				std::stringstream	buffer;
				buffer << file.rdbuf();
				file.close();
				
				// init body response avec \r\n\r\n de fin
				this->_response = buffer.str() + "\r\n\r\n";

				// OUT
				return ;
			}
			// si file not open && next == end vector Aucun index valable
			else if (it + 1 == serv._index.end())
				throw 404; // <<---- BAD CONFIG FILE NOT 404 (trouver l'erreur précise)
		}
	}

	// Else si l'adresse n'est pas home 
	// evaluation de l'adresse
	request_loc = serv._root + &request_loc[1];
	std::ifstream file(request_loc);
	file.open(request_loc, std::ios::in);
	
	// si s'ouvre pas retour 404 Not found
	if (!file.is_open())
		throw 404;

	// get body response idem L 138 - 144
	std::stringstream	buffer;
	buffer << file.rdbuf();
	file.close();
	this->_response = buffer.str() + "\r\n\r\n";
}

void	Http_handler::__err_header(int ret)
{
	this->header_http1 += std::to_string(ret) + " " + "Not Found\r\n";
	this->header_content_len += std::to_string(0) + "\r\n";
	this->header_content_loc += this->_address + "\r\n";
	this->header_content_type += "ico\r\n"; // <<-- TODO get type
	this->header_date += this->__get_time() + "\r\n";

	this->_header += this->header_http1 + this->header_content_len
				+ this->header_content_loc + this->header_content_type
				+ this->header_date + this->header_server + "\r\n"
				+ this->header_encoding + "\r\n"
				+ "\r\n";
	
	this->_response = this->_header + "\r\n\r\n";
}

std::string	Http_handler::__get_time()
{
	time_t		rawtime;
	struct tm	*timeinfo;
	
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);

	return (std::string(buffer));
}
