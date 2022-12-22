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
	std::string	version;

	firstLineStream >> this->_method >> this->_address >> version;
	this->__clean_address();

	this->_req_dict.insert(std::make_pair(this->_method, this->_address));

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
	struct stat	path_stat;
	bool		cgi = false;

	try
	{
		// GET METHOD
		if ( (val = this->_req_dict.find("GET")) != this->_req_dict.end() )
		{
			if (!SV_GETISSET(serv))
				throw 406;
			
			if (serv.cgi_exec(this->_address))
				this->__CGI_exec(serv._root + &this->_address[1], serv);
			else
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

			if (cgi)
				throw 403;

			int ret = this->__POST_response(val->second, serv);
			this->__200_response(ret);
		}

		// DELETE_METHOD
		else if ( (val = this->_req_dict.find("DELETE")) != this->_req_dict.end() )
		{
			if (!SV_DELETEISSET(serv))
				throw 406;

			if (cgi)
				throw 403;

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
	this->header_connect += "close\r\n";

	if (!this->_response.empty())
	{
		this->header_content_len += std::to_string(this->_response.length() + 2) + "\r\n";
		this->header_content_type += "text\r\n";
		
		this->_header += this->header_http1
					+ this->header_content_len
					+ this->header_content_loc
					+ this->header_content_type
					+ this->header_date
					+ this->header_connect
					+ this->header_server + "\r\n"
					+ this->header_encoding + "\r\n";

		this->_response = this->_header + "\r\n" + this->_response + "\r\n";
	}
	else
	{
		this->_header += this->header_http1
					+ this->header_content_loc
					+ this->header_date
					+ this->header_connect
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
	this->header_connect += "close\r\n";
	
	this->_header += this->header_http1
				+ this->header_content_loc
				+ this->header_content_len
				+ this->header_content_type
				+ this->header_date
				+ this->header_connect
				+ this->header_server + "\r\n"
				+ this->header_encoding + "\r\n";

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
	std::string	request_loc(this->_address);

	// HOME ----------------------------------------------------------------	
	// Si l'adresse est home -> '/'  look dans config files les index
	if  (request_loc == "/" && !SV_DIRISACTIVE(serv))
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
				//read / get file and close file
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

	std::ifstream file;
	if (request_loc == "/autoindex" || request_loc == "/")
	{
		request_loc = serv._root;
		file.open(request_loc, std::ios::in);
		if (!file.is_open())
			throw 500;
	}
	else
	{
		request_loc = serv._root + &request_loc[1];
		file.open(request_loc, std::ios::in);
		if (!file.is_open())
			throw 404;
	}

	// évalue si c'est un directory
	struct stat	path_stat;
	stat(request_loc.c_str(), &path_stat);

	if (S_ISDIR(path_stat.st_mode))
	{
		if (!SV_DIRISACTIVE(serv))
			throw 403;
		__directory_browser(request_loc.c_str(), this->_address);
		file.close();
		return ;
	}
		
	// get body response
	if (file.rdbuf()->in_avail() == 0)
	{
		file.close();
		return ;
	}
	//There are readable characters remaining in the file
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

	int ret;
	
	std::string	path = serv._root + &this->_address[1];
	std::fstream	file(path, std::ios::out);
	if (!file.is_open())
		throw 404;

	struct stat path_stat;
	stat(path.c_str(), &path_stat);

	if (S_ISDIR(path_stat.st_mode))
		this->__close_and_throw(file, 401);

	if (S_ISREG(path_stat.st_mode))
		ret = 202;
	else
		ret = 201;

	if (this->_req_dict.find("Content-Type")->second.find("multipart/form-data")
			== std::string::npos)
		this->__close_and_throw(file, 400);
	
	if (serv._body_max_size > 0
		&& std::atol(this->_req_dict.find("Content-Length")->second.c_str())
			> serv._body_max_size)
		this->__close_and_throw(file, 413);
	
	if (serv._body_min_size > 0
		&& std::atol(this->_req_dict.find("Content-Length")->second.c_str())
			> serv._body_min_size)
		this->__close_and_throw(file, 400);

	file << this->_req_dict.find("BODY")->second;
	file.close();

	return (ret);
}

void	Http_handler::__DELETE_response(std::string &value, Server &serv)
{
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











// CGI_PART
void	Http_handler::__CGI_exec(const std::string path, Server &serv)
{
	int pipefd[2];

	if (pipe(pipefd) == -1)
		throw 500;
	
	pid_t	pid = fork();
	if (pid == -1)
		throw 500;
	else if (pid == 0) // child
	{
		char *argv[3];

		if (close(pipefd[0]) == -1 // Close la partie read
			|| dup2(pipefd[1], STDOUT_FILENO) == -1 // Redirect stdout + str sur la partie write de pipe
			|| dup2(pipefd[1], STDERR_FILENO) == -1)
			exit(EXIT_FAILURE);

		std::string	tmp(serv.get_cgi_path(path));

		argv[0] = const_cast<char *>(tmp.c_str());
		argv[1] = const_cast<char *>(path.c_str());
		argv[2] = NULL;

		execve(argv[0], argv, serv._env);
		exit(EXIT_FAILURE);	
	}

	else // parent
	{
		if (close(pipefd[1]) == -1) // close la partie write
			throw 500;
		int	status;
		waitpid(pid, &status, 0);

		if (WIFEXITED(status)) // child process terminated normally
		{
			int exitCode = WEXITSTATUS(status);
			if (exitCode != 0) // the child process exited with error code
				throw 500;
		}
		else
			throw 500; // The child process terminated abnormally (e.g. due to a signal)

		char buffer[1048];
		ssize_t	n;
		std::cout << "WTF" << std::endl;
		while ((n = read(pipefd[0], buffer, 1048)) > 0)
			this->_response.append(buffer, n);

		if (close(pipefd[0]) == -1)
		{
			this->_response.clear();
			throw 500;
		}
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
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir))
	{
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

	if (host == "/autoindex" || host == "/")
    	ss << "<p><a href=/"  << dirEntry + '>' + dirEntry + "</a></p>\n";
	else
   		ss << "<p><a href=" + host + '/' << dirEntry + '>' + dirEntry + "</a></p>\n";
    return ss.str();
}











// UTILS
void	Http_handler::__clean_address()
{
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

void	Http_handler::__close_and_throw(std::fstream &file, int err)
{
	file.close();
	throw err;
}
