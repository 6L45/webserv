#include "http_handler.hpp" 

t_errs		g_errs;
t_ret		g_ret;
memelord	g_gif;

// CONSTRUCT
Http_handler::Http_handler(std::string &request)
{
	std::cout << request << std::endl << std::endl;
	
	// Read the first line from the string stream
	std::istringstream iss(request);
	std::string firstLine;
	std::getline(iss, firstLine);

	// Split the first line into its individual components
	std::istringstream firstLineStream(firstLine);
	std::string address, version;

	firstLineStream >> this->_method >> address >> version;
	this->_req_dict.insert(std::make_pair(this->_method, address));

	std::string line;
	while (std::getline(iss, line))
	{
		// Extract the field name and value from each line
		std::size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string field = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);
			if (value[0] == ' ')
				value = value.substr(1);
			this->_req_dict.insert(std::make_pair(field, value));
		}
	}

	if (version != "HTTP/1.1")
		this->_valid = 505;
	else if (this->_req_dict.find("Host") == this->_req_dict.end())
		this->_valid = 400;
	else
		this->_valid = 0;

	size_t pos = request.find("\r\n\r\n");
	if (pos == std::string::npos)
		return;
	// Extract the body from the request
	this->_req_dict.insert(std::make_pair("BODY", request.substr(pos + 4)));
}











// PUBLIC METHOD
std::string	Http_handler::exec_request(Server &serv)
{
	typename Http_handler::MMAPIterator	val;

	try
	{
		// GET METHOD
		if ( (val = this->_req_dict.find("GET")) != this->_req_dict.end() )
		{
			if (!SV_GETISSET(serv))
				throw 406;

			this->__GET_response(val->second, serv);

			if (!this->_response.empty())
				this->__200_response(200);
			else
				this->__200_response(204);
		}

		// POST_METHOD
		else if ( (val = this->_req_dict.find("POST")) != this->_req_dict.end() )
		{
			if (!SV_POSTISSET(serv))
				throw 406;

			int ret = this->__POST_response(val->second, serv);
			this->__200_response(ret);
		}

		// DELETE_METHOD
		else if ( (val = this->_req_dict.find("DELETE")) != this->_req_dict.end() )
		{
			if (!SV_DELETEISSET(serv))
				throw 406;

			this->__DELETE_response(val->second, serv);
			this->__200_response(204);
		}

		// ANY OTHER TYPE OF METHOD -> 406
		else if (this->_method == "OPTIONS" ||this->_method == "HEAD"
				||this->_method == "PUT" || this->_method == "PATCH")
			throw 406;
		// BAD REQUEST NO METHOD OR BS METHOD
		else
			throw 400;
	}
	catch (int e) {
		__err_header(e);
	}

	return (this->_response);
}

std::string	Http_handler::get_host_name() const {			// GET HOST NAME
	return (this->_req_dict.find("Host")->second);
}

std::pair<typename Http_handler::MMAPConstIterator,
			typename Http_handler::MMAPConstIterator>
	Http_handler::get_elems(const std::string key) const {	// GET ELEMS
		return (this->_req_dict.equal_range(key));
}

int	Http_handler::invalid_request() const {
	return (this->_valid);
}

std::string	Http_handler::bad_request(void)
{
	this->__err_header(_valid);
	return (this->_response);
}











// response generator
void	Http_handler::__200_response(int ret)
{
	this->header_http1 += std::to_string(ret) + " " + g_ret[ret] + "\r\n";
	this->header_content_loc += this->_address + "\r\n";
	this->header_date += this->__get_time() + "\r\n";

	if (!this->_response.empty())
	{
		this->header_content_len += std::to_string(this->_response.length() + 2) + "\r\n";
		this->header_content_type += "text\r\n";
		
		this->_header += this->header_http1
					+ this->header_content_len
					+ this->header_content_loc
					+ this->header_content_type
					+ this->header_date
					+ this->header_server + "\r\n"
					+ this->header_encoding + "\r\n";

		this->_response = this->_header + "\r\n" + this->_response + "\r\n";
	}
	else
	{
		this->_header += this->header_http1
					+ this->header_content_loc
					+ this->header_date
					+ this->header_server + "\r\n"
					+ this->header_encoding + "\r\n";

		this->_response = this->_header + "\r\n";
	}
}

void	Http_handler::__err_header(const int ret)
{
	this->__body_gen(ret);

	this->header_http1 += std::to_string(ret) + " " + g_errs[ret] + "\r\n";
	this->header_content_len += std::to_string(this->_response.length() + 2) + "\r\n";
	this->header_content_loc += this->_address + "\r\n";
	this->header_content_type += "text\r\n";
	this->header_date += this->__get_time() + "\r\n";
	
	this->_header += this->header_http1
				+ this->header_content_loc
				+ this->header_content_len
				+ this->header_content_type
				+ this->header_date
				+ this->header_server + "\r\n" + this->header_encoding + "\r\n";

	this->_response = this->_header + "\r\n"  + this->_response + "\r\n";
}

void	Http_handler::__body_gen(int ret)
{
	this->_response += "<!DOCTYPE html>\r\n";
	this->_response += "<html>\r\n";
	this->_response += "	<head>\r\n";
	this->_response += "		<meta charset=\"UTF-8\">\r\n";
	this->_response += "		<title>webserv</title>\r\n";
	this->_response += "	</head>\r\n";
	this->_response += "	<body bgcolor=\"white\">\r\n";
	this->_response += "		<center><h1>" + std::to_string(ret) + " " + g_errs[ret] + "</h1></center>\r\n";
	this->_response += "		<center><hr>" + this->header_server + "</hr></center>\r\n";
	this->_response +=			g_gif[1];
	this->_response += "		<center><p>Click <a href=\"/\">here</a> to return home.</p></center>\r\n";
	this->_response += "	</body>\r\n";
	this->_response += "</html>\r\n";
}











// RESPONSES
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
	if (!file.is_open())
		throw 404;

	// évalue si c'est un directory
	struct stat	path_stat;
	stat(request_loc.c_str(), &path_stat);

	if (S_ISDIR(path_stat.st_mode))
	{
		__directory_browser(request_loc.c_str(), this->_address);
		file.close();
		return ;
	}
		
	// get body response
	if (file.peek() != EOF)
	{
		file.close();
		return ;
	}
	// There are readable characters remaining in the file
	std::stringstream	buffer;
	buffer << file.rdbuf();
	file.close();
	this->_response = buffer.str() + "\r\n";
}

int	Http_handler::__POST_response(std::string &value, Server &serv)
{
	if (this->_req_dict.find("Content-Length") == this->_req_dict.end()
		|| this->_req_dict.find("Content-Type") == this->_req_dict.end() )
		throw 411;

	this->__init_response(value);

	int ret;
	struct stat path_stat;
	std::string	path = serv._root + &this->_address[1];
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











// DIRECTORY _BROWSER
void         Http_handler::__directory_browser(const char *path, std::string const &host)
{
    std::string	dirName(path);
    DIR *dir	= opendir(path);

    std::string page =\
"<!DOCTYPE html>\r\n\
<html>\r\n\
<head>\r\n\
<title>" + dirName + "</title>\r\n\
</head>\r\n\
<body>\r\n\
<h1>INDEX</h1>\r\n\
<p>\n";

    if (dirName[0] != '/')
        dirName = "/" + dirName;
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) {
        page += __filesLst(std::string(dirEntry->d_name), dirName, host);
    }

    page +="\
</p>\r\n\
</body>\r\n\
</html>\r\n";

    closedir(dir);
    this->_response = page;
}

std::string         Http_handler::__filesLst(std::string const &dirEntry, std::string const &dirName, std::string const &host)
{
    std::stringstream   ss;

    ss << "<p><a href=" + host + '/' << dirEntry + '>' + dirEntry + "</a></p>\n";
    return ss.str();
}











// UTILS
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
