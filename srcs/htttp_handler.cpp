#include "http_handler.hpp" 

// code error request
t_errs	g_errs;

// code success request
t_ret	g_ret;

Http_handler::Http_handler(std::string &request)
{
	std::stringstream	req(request.c_str());
	std::string			line;

	std::cout << request << std::endl;

	while (std::getline(req, line, '\n'))
	{
		int			index;
		std::string	key;
		
		if (line[0] == '\n' && line[1] == '\n')
			break;
		
		index = line.find(':');
		if (index == -1)
		{
			index = line.find(' ');
			if (index == -1)
				break;
		}
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

	int index = request.find("\n\n");
	if (index > 0)
	{
		std::string	body(&request[index + 2]);
		body = body.substr(0, body.find("\n\n"));
		this->_req_dict.insert(std::make_pair("BODY", body));
	}
}

std::string	Http_handler::exec_request(Server &serv)
{
	typename Http_handler::MMAPIterator	val;

	try
	{
		if ( (val = this->_req_dict.find("GET")) != this->_req_dict.end() )
		{
			if (!SV_GETISSET(serv))
				throw 406;
			this->__GET_method(val->second, serv);
		}
		else if ( (val = this->_req_dict.find("POST")) != this->_req_dict.end() )
		{
			if (!SV_POSTISSET(serv))
				throw 406;
			this->__POST_method(val->second, serv);
		}
		else if ( (val = this->_req_dict.find("DELETE")) != this->_req_dict.end() )
		{
			if (!SV_DELETEISSET(serv))
				throw 406;
			this->__DELETE_method(val->second, serv);
		}
		else
			throw 400;
	}
	catch (int e) {
		__err_header(e);
	}

	return (this->_response);
}

std::string	Http_handler::get_host_name() const {
	return (this->_req_dict.find("Host")->second);
}


std::pair<typename Http_handler::MMAPConstIterator,
			typename Http_handler::MMAPConstIterator>
	Http_handler::get_elems(const std::string key) const {
		return (this->_req_dict.equal_range(key));
}


void	Http_handler::__GET_method(std::string &value, Server &serv)
{
	this->__GET_response(value, serv);
	
	this->header_content_loc += this->_address + "\r\n";
	this->header_date += this->__get_time() + "\r\n";
	
	if (!this->_response.empty())
	{
		this->header_http1 += "200 OK\r\n";
		this->header_content_len += std::to_string(this->_response.length() - 4) + "\r\n";
		this->header_content_type += "text\r\n";
		
		this->_header += this->header_http1 + this->header_content_len
					+ this->header_content_loc + this->header_content_type
					+ this->header_date + this->header_server + "\r\n"
					+ this->header_encoding + "\r\n";

		if  (!this->_response.empty())	
			this->_response = this->_header + "\r\n" + this->_response + "\r\n";
		else
			this->_response = this->_header + "\r\n";
	}
	else
	{
		this->header_http1 += "204 No Content\r\n";
		
		this->_header += this->header_http1
					+ this->header_content_loc + this->header_date
					+ this->header_server + "\r\n" + this->header_encoding + "\r\n";
	
		if  (!this->_response.empty())	
			this->_response = this->_header + "\r\n" + this->_response + "\r\n";
		else
			this->_response = this->_header + "\r\n";	
	}
}

void	Http_handler::__POST_method(std::string &value, Server &serv)
{
	int ret = this->__POST_response(value, serv);

	this->header_http1 += std::to_string(ret) + " " + g_ret[ret] + "\r\n";
	this->_header += this->header_http1
				+ this->header_content_loc + this->header_date
				+ this->header_server + "\r\n" + this->header_encoding + "\r\n";
	
	if  (!this->_response.empty())	
		this->_response = this->_header + "\r\n" + this->_response + "\r\n";
	else
		this->_response = this->_header + "\r\n";	
}

void	Http_handler::__DELETE_method(std::string &value, Server &serv)
{
	this->__DELETE_response(value, serv);
	
	this->header_content_loc += this->_address + "\r\n";
	this->header_date += this->__get_time() + "\r\n";
	this->header_http1 += "204 No Content\r\n";
	
	this->_header += this->header_http1
				+ this->header_content_loc + this->header_date
				+ this->header_server + "\r\n" + this->header_encoding + "\r\n";

	this->_response = this->_header + "\r\n";	
}


void	Http_handler::__GET_response(std::string &value, Server &serv)
{
	this->__init_response(value);
	std::string	request_loc(this->_address);

	// HOME ----------------------------------------------------------------	
	// Si l'adresse est home -> '/'  look dans config files les index
	if  (!request_loc.compare("/"))
	{
		// parcours les index
		for (std::vector<std::string>::iterator it = serv._index.begin();
				it != serv._index.end(); it++)
		{
			std::ifstream	file;
			// teste ouverture des index
			file.open(serv._root + *it, std::ios::in);
			if (file.is_open())
			{
				// read / get file and close file
				if (file.rdbuf()->in_avail() == 0)
				{
					file.close();
					return ;
				}
				std::stringstream	buffer;
				buffer << file.rdbuf();
				file.close();
				
				// init body response avec \r\n\r\n de fin
				this->_response = buffer.str() + "\r\n";

				// OUT
				return ;
			}
			// si file not open && next == end vector Aucun index valable
			else if (it + 1 == serv._index.end())
				throw 500; // Internal server error
		}
	}

	// QUELQUE PART DANS LE SITE --------------------------------------------------
	request_loc = serv._root + &request_loc[1];

	std::ifstream file(request_loc);
	file.open(request_loc, std::ios::in);
	
	// évalue si c'est un directory
	struct stat	path_stat;
	stat(request_loc.c_str(), &path_stat);

	if (S_ISDIR(path_stat.st_mode))
	{
		directory_browser(request_loc.c_str(), this->_address);
		file.close();
		return ;
	}

	// si s'ouvre pas retour 404 Not found
	if (!file.is_open())
		throw 404;

	// get body response
	if (file.rdbuf()->in_avail() == 0)
	{
		file.close();
		return ;
	}
	std::stringstream	buffer;
	buffer << file.rdbuf();
	file.close();
	this->_response = buffer.str() + "\r\n";
}

int	Http_handler::__POST_response(std::string &value, Server &serv)
{
	if (this->_req_dict.find("Content-Length") == this->_req_dict.end() )
		throw 411;
	if (this->_req_dict.find("Content-Type") == this->_req_dict.end() )
		throw 411;

	this->__init_response(value);


	int ret;
	struct stat path_stat;

	std::string	path = serv._root + &this->_address[1];
	std::cout << "---> " << path << std::endl;

	stat(path.c_str(), &path_stat);
	if (S_ISDIR(path_stat.st_mode))
		throw 401;
	if (S_ISREG(path_stat.st_mode))
		ret = 202;
	else
		ret = 201;
/*
	if (serv.max_size > 0 && std::atol(this->_req_dict.find("Content-Length")->second.c_str()) > serv.max_size)
		throw 413; // Max size exceeded
	if (serv.min_size > 0 && std::atol(this->_req_dict.find("Content-Length")->second.c_str()) > serv.min_size)
		throw 400; // min size not met
*/
	std::fstream	file(path, std::ios::out);
	if (!file.is_open())
		throw 500;
	file << this->_req_dict.find("BODY")->second;
	file.close();

	return (ret);
}

void	Http_handler::__DELETE_response(std::string &value, Server &serv)
{
	this->__init_response(value);

	struct stat path_stat;
	std::string	path = (serv._root + &this->_address[1]);
	stat(path.c_str(), &path_stat);

	if (S_ISDIR(path_stat.st_mode))
		throw 400;

	int result = remove(path.c_str());
	if (result)
	{
		if (result == -1)
			throw 404;
		else
			throw 500;
	}
}

void	Http_handler::__err_header(const int ret)
{
	this->header_http1 += std::to_string(ret) + " " + g_errs[ret] + "\r\n";
	this->header_content_loc += this->_address + "\r\n";
	this->header_date += this->__get_time() + "\r\n";
	this->_header += this->header_http1 + this->header_content_len
				+ this->header_content_loc + this->header_date
				+ this->header_server + "\r\n" + this->header_encoding + "\r\n";

//	this->_response = html_template[ret]; <<-- SI PAGES HTML PREDEFINIES PAR ERREUR
	this->_response = this->_header + "\r\n" /* + this->_response + "\r\n" */ ;
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

void         Http_handler::directory_browser(const char *path, std::string const &host)
{
    std::string	dirName(path);
    DIR *dir	= opendir(path);

    std::string page =\
"<!DOCTYPE html>\n\
<html>\n\
<head>\n\
<title>" + dirName + "</title>\n\
</head>\n\
<body>\n\
<h1>INDEX</h1>\n\
<p>\n";

    if (dirName[0] != '/')
        dirName = "/" + dirName;
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) {
        page += filesLst(std::string(dirEntry->d_name), dirName, host);
    }

    page +="\
</p>\n\
</body>\n\
</html>\n";

    closedir(dir);
    this->_response = page;
}

std::string         Http_handler::filesLst(std::string const &dirEntry, std::string const &dirName, std::string const &host)
{
    std::stringstream   ss;

    ss << "<p><a href=" + host + '/' << dirEntry + '>' + dirEntry + "</a></p>\n";
    return ss.str();
}

void	Http_handler::__init_response(std::string &value)
{
	int index = value.find("HTTP");
	this->_address = value.substr(0, index - 1);

	if (this->_address.compare("/"))
	{
		int i = this->_address.length() - 1; 
		while (i > 0 && this->_address[i] == '/')
		{
			this->_address.erase(i);
			i--;
		}
	}
}
