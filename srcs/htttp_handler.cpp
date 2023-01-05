#include "http_handler.hpp" 

TODO virer \r\n\r\n a la fin du body
et Content-range on cut


t_errs		g_errs;
t_ret		g_ret;
t_ext		g_ext;
memelord	g_gif;

// CONSTRUCT
Http_handler::Http_handler(std::string &request)
{
	std::cout << "---------------------------------------------- REQUEST" << std::endl;
	std::cout << request << std::endl;
	std::cout << "------------------------------------------------------" << std::endl;

	// Read the first line from the string stream
	std::istringstream iss(request);
	std::string firstLine;
	std::getline(iss, firstLine);
	this->_valid = 0;

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
		std::size_t	colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string field = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);
			if (!value.empty() && value.back() == '\r')
				value.pop_back();
			if (value[0] == ' ')
				value = value.substr(1);
			this->_req_dict.insert(std::make_pair(field, value));
		}
	}
	if (this->_valid == 0)
	{
		if (this->_method.empty() || this->__not_a_method())
			this->_valid = 400;
		else if (version != "HTTP/1.1")
			this->_valid = 505;
		else if (this->_req_dict.find("Host") == this->_req_dict.end())
			this->_valid = 400;
	}

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
		__this_is_the_way(serv);
		// GET METHOD
		if ( ((val = this->_req_dict.find("GET")) != this->_req_dict.end() )
			|| ( (val = this->_req_dict.find("HEAD")) != this->_req_dict.end() ) )
		{
			if (!val->first.compare("GET") && !SV_GETISSET(serv))
//				|| (!val->second.compare("HEAD") && !SV_HEADISSET(serv)))
				throw 405;
	
			__Etag_gen(serv._root + &this->_address[1]);

			std::map<std::string, std::string>::iterator	it;

			it = this->_req_dict.find("ETag");
			if (it != this->_req_dict.end())
				__Etag_reader(it->second);

			it = this->_req_dict.find("If-Match");
			if (it != this->_req_dict.end())
				__Etag_reader(it->second);

			it = this->_req_dict.find("If-Modified-Since");
			if (it != this->_req_dict.end())
				__condition_header(serv._root + &this->_address[1], MODIFIED_SINCE);

			if (serv.cgi_exec(this->_address))
				this->__CGI_exec(serv._root + &this->_address[1], serv);
			else
				this->__GET_response(val->second, serv);
			
			it = this->_req_dict.find("Range");
			if (it != this->_req_dict.end())
			{
				__range_cut(it->second);
				this->__200_response(206, false, serv);
			}
			else if (!val->first.compare("GET"))
				this->__200_response(200, false, serv);
			else
				this->__200_response(200, true, serv);
		}

		// POST_METHOD
		else if ( (val = this->_req_dict.find("POST")) != this->_req_dict.end() )
		{
			if (!SV_POSTISSET(serv))
				throw 405;

			if (cgi)
				throw 403;

			int ret = this->__POST_response(val->second, serv);
			this->__200_response(ret, false, serv);
		}

		// DELETE_METHOD
		else if ( (val = this->_req_dict.find("DELETE")) != this->_req_dict.end() )
		{
			if (!SV_DELETEISSET(serv))
				throw 405;

			if (cgi)
				throw 403;

			this->__DELETE_response(val->second, serv);
			this->__200_response(204, false, serv);
		}
	
		// ANY OTHER TYPE OF METHOD -> 405
		else if (!this->_method.compare("OPTIONS") || !this->_method.compare("PUT")
				|| !this->_method.compare("PATCH") || !this->_method.compare("TRACE")
				|| !this->_method.compare("CONNECT") || !this->_method.compare("PATCH"))
			throw 405;
		// BAD REQUEST NO METHOD OR BS METHOD
		else
			throw 400;
	}
	catch (int e) {
		__err_header(e);
	}

	return (this->_response);
}











// GETTER
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

std::string	Http_handler::get_connection() const
{
	std::map<std::string, std::string>::const_iterator	it = this->_req_dict.find("Connection");

	if (it != this->_req_dict.end())
		return (it->second);
	return (std::string("null"));
}

bool	Http_handler::get_keep_alive() const {
	return (this->_req_dict.find("Keep-Alive") != this->_req_dict.end());
}

int	Http_handler::keep_alive_value() const
{
	int result;
	std::map<std::string, std::string>::const_iterator	it;

	it = this->_req_dict.find("Keep_Alive");
	std::istringstream(it->second) >> result;
	if (std::istringstream(it->second).fail())
		throw 400;;

	return result;
}











// response generator
void	Http_handler::__200_response(int ret, bool head, Server &serv)
{
	bool	bin = false;

	this->header_http1 += std::to_string(ret) + " " + g_ret[ret] + "\r\n";
	this->header_content_loc += this->_address + "\r\n";
	this->header_date += Http_handler::get_time() + "\r\n";
	this->header_connect += "close\r\n";
	
	if (!this->header_content_type.compare("Content-Type: "))
	{
		if (g_ext.get_type((serv._root + &this->_address[1]), this->__get_extension()).empty())
			this->header_content_type += "text/html\r\n";
		else
			this->header_content_type += g_ext.get_type((serv._root + &this->_address[1]), this->__get_extension()) + "\r\n";

	}
	if (this->_response.empty() && !this->header_content_len.compare("Content-Length: "))
		this->header_content_len += "0\r\n";
	else if (!this->header_content_len.compare("Content-Length: "))
		this->header_content_len += std::to_string(this->_response.length() + 2) + "\r\n";
	else
		bin = true;

	this->_header += this->header_http1
				+ this->header_content_len
				+ this->header_content_loc
				+ this->header_content_type
				+ this->header_date
				+ this->header_connect
				+ this->header_server + "\r\n"
				+ this->header_encoding + "\r\n";

	if (!this->header_ETag.empty())
		this->_header += this->header_ETag + "\r\n";

	if (bin)
	{
		if (!head)
			this->_response = this->_header + "\r\n" + this->_response;
		else
			this->_response = this->_header + "\r\n";
			
	}
	else if (!head && !this->_response.empty())
		this->_response = this->_header + "\r\n" + this->_response + "\r\n";
	else
		this->_response = this->_header + "\r\n";
	
	std::cout << this->_header << std::endl;
}

void	Http_handler::__err_header(const int ret)
{
	this->__body_gen(ret);

	this->header_http1 += std::to_string(ret) + " " + g_errs[ret] + "\r\n";
	this->header_content_len += std::to_string(this->_response.length() + 2) + "\r\n";
	this->header_content_loc += this->_address + "\r\n";
	this->header_content_type += "text/html\r\n";
	this->header_date += Http_handler::get_time() + "\r\n";
	this->header_connect += "close\r\n";
	
	this->_header += this->header_http1
				+ this->header_content_loc
				+ this->header_content_len
				+ this->header_content_type
				+ this->header_date
				+ this->header_connect
				+ this->header_server + "\r\n";

	if (ret == 300)
	{
		this->_header += this->header_alternate + "\r\n";
		this->_header += this->header_transfer + "\r\n";
	}
	else
		this->_header += this->header_encoding + "\r\n";
	if (ret == 301)
	{
		this->header_location += this->_address + "/\r\n";
		this->_header += this->header_location;
	}
	if (!this->header_ETag.empty())
		this->_header += this->header_ETag + "\r\n";

	if (ret == 304)
		this->_response = this->_header + "\r\n";
	else
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
	this->_response +=			g_gif[1] + "\r\n";
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
	if  (!request_loc.compare("/") && !SV_DIRISACTIVE(serv))
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

				return ;
			}
			// si file not open && next == end vector Aucun index valable
			else if (it + 1 == serv._index.end())
				throw 500; // Internal server error
		}
	}

	// QUELQUE PART DANS LE SITE --------------------------------------------------

	std::ifstream	file;
	struct stat		path_stat;

	if (!request_loc.compare("/autoindex") || !request_loc .compare("/"))
	{
		request_loc = serv._root;
		file.open(request_loc, std::ios::in);
		if (!file.is_open())
			throw 500;
	}
	else if (g_ext.get_category((serv._root + &request_loc[1]), this->__get_extension())
				== BIN)
	{
		request_loc = serv._root + &request_loc[1];
		file.open(request_loc, std::ios::binary);
		if (!file.is_open())
			throw 404;

		// évalue si c'est un directory
		stat(request_loc.c_str(), &path_stat);

		if (S_ISDIR(path_stat.st_mode))
		{
			if (this->_address[this->_address.length() - 1] != '/')
			{
				file.close();
				throw 301;
			}
			if (!SV_DIRISACTIVE(serv))
			{
				file.close();
				throw 403;
			}
			else
			{
				for (std::vector<std::string>::iterator it = serv._index.begin();
						it != serv._index.end(); it++)
				{
					std::ifstream	file;
					// teste ouverture des index
					file.open(request_loc + *it, std::ios::in);
					if (file.is_open())
					{
						this->_address = this->_address + *it;
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

						return ;
					}
					else if (it + 1 == serv._index.end())
						__directory_browser(request_loc.c_str(), this->_address);
				}
			}
			this->header_content_type += "text/html\r\n";
			file.close();
			return ;
		}

		std::stringstream	buffer;
		buffer << file.rdbuf();
		file.close();
		this->_response = buffer.str();
		std::cout << 8 << std::endl;

		this->header_content_len += std::to_string(this->_response.length()) + "\r\n";
		std::cout << 9 << std::endl;

		return;
	}
	else
	{
		request_loc = serv._root + &request_loc[1];
		file.open(request_loc, std::ios::in);
		if (!file.is_open())
			throw 404;
	}

	
		
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
	std::string	path = serv._root + &this->_address[1];

	if (this->_req_dict.find("BODY") == this->_req_dict.end())
		throw 501;

	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0)
		throw 404;

	int ret;
	if (S_ISDIR(path_stat.st_mode))
		throw 403;

	std::fstream	file(path, std::ios::out);
	if (!file.is_open())
		throw 500;

	if (S_ISREG(path_stat.st_mode))
		ret = 202;
	else
		ret = 201;

	if (this->_req_dict.find("Content-Length") == this->_req_dict.end()
		|| this->_req_dict.find("Content-Type") == this->_req_dict.end() )
		this->__close_and_throw(file, 411);

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
		while ((n = read(pipefd[0], buffer, 1048)) > 0)
			this->_response.append(buffer, n);

		if (close(pipefd[0]) == -1)
		{
			this->_response.clear();
			throw 500;
		}
	}
}

std::string	get_extension(const std::string &path)
{
	std::string	extension;
	size_t		dot_pos = path.find_last_of('.');

	if (dot_pos != std::string::npos)
		extension = path.substr(dot_pos + 1);

	// Convert the extension to lowercase
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	return extension;
}

void	trim(std::string &str)
{
	// Trim leading whitespace
	str.erase(str.begin(), std::find_if(str.begin(), str.end(),
			[](int ch) {
				return !std::isspace(ch);
			}));

	// Trim trailing whitespace
	str.erase(std::find_if(str.rbegin(), str.rend(),
			[](int ch) {
				return !std::isspace(ch);
			}).base(), str.end());
}

std::vector<std::string>	cpp_split(std::string &src, char sep)
{
	std::vector<std::string>	substrings;
	std::string					current;

	for (int i = 0; i < src.length(); i++)
	{
		if (src[i] == sep || i == src.length() - 1)
		{
			if (!current.empty())
			{
				trim(current);
				substrings.push_back(current);
			}
			current.clear();
		}
		else
			current += src[i];
	}
	return (substrings);
}



// PRIVATE METHODS
void	Http_handler::__this_is_the_way(Server &serv)
{
	std::string	path;
	std::string	directory;
	struct stat file_stat;

	directory =  (serv._root + &this->_address[1])
					.substr(0, (serv._root + &this->_address[1]).find_last_of('/'));

	// Si il y'a field accept dans la requete
	if (this->_req_dict.find("Accept") != this->_req_dict.end()
		&& get_extension(this->_address).empty())
	{
		std::string	vals;
		vals = this->_req_dict.find("Accept")->second;
		if (!vals.compare("*/*"))
			return ;

		// verify directory where file is situated.
		if (stat(directory.c_str(), &file_stat) != 0)
			throw 404;

		// Accept Parsing --------
		// multimap [(float)q=x.y] -> std::string (val)
		std::vector<std::string>			accept_field_pairs = cpp_split(vals, ',');
		std::multimap<float, std::string>	accept_vals;
		
		for (std::vector<std::string>::iterator it = accept_field_pairs.begin();
				it != accept_field_pairs.end(); it++)
		{
			int	sep_pos = it->find(';');
			int	q_pos = it->find("q=");
			
			accept_vals.insert(std::make_pair(std::stof(it->substr(q_pos + 2)),
												it->substr(0, sep_pos)));
		}
		// ------------------------

		// pour chaques element dans multimap tester les valeurs demandées
		// reverse iterator pour l'ordre de priorité établie par la valeur q
		float	q = 0.9;
		bool	err300 = false;
		for (std::multimap<float, std::string>::reverse_iterator it = accept_vals.rbegin();
				it != accept_vals.rend(); it++)
		{
			int	slash_pos = it->second.find('/');
			
			if (it->second[slash_pos + 1] == '*')
			{
    			DIR			*dir = opendir(directory.c_str());
				std::string	valid_ext;
				
				// verifie l'ensemble des fichiers présent dans le répertoire si il sont du type demandé (image / video / application / texte)
				for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir))
				{
					// si match
					if (g_ext.get_type(get_extension(dirEntry->d_name))
						.find(it->second.substr(0, slash_pos)) != std::string::npos)
					{
						// si valid_ext (valid extension) not empty throw 300 multiple choice
						if (!valid_ext.empty())
							err300 = true;
						// else valid_ext = extension du fichier matched
						valid_ext = "." + get_extension(dirEntry->d_name);
						this->header_alternate += g_ext.get_type(&valid_ext[1]) + "; q=" + std::to_string(q) + " , ";
						if (q > 0.1)
							q -= 0.1;
						
						if (this->header_transfer.empty())
							this->header_transfer = "Transfer-Encoding: chunked";
					}
				}
				if (err300)
				{
					this->header_alternate.erase(this->header_alternate.end() - 3, this->header_alternate.end());
					throw 300;
				}
				// q determine l'ordre de priorité si un fichier a été trouvé
				// pas besoin de check autre possibilitées
				if (!valid_ext.empty())
				{
					path = this->_address + valid_ext;
					break;
				}
			} // if wildcard

			else
			{
				if (!path.empty())
					break;

				// serv._root + path request + . + extension du accept
				std::string	new_path = serv._root + &this->_address[1] + "." +
										it->second.substr(slash_pos + 1);

				if (stat(new_path.c_str(), &file_stat) == 0)
					path = this->_address + "." + it->second.substr(slash_pos + 1);
			} // if not a wildcard

		}

		if (path.empty())
			throw 404;
		this->_address = path;

	} // if accept field
}

void	Http_handler::__directory_browser(const char *path, std::string const &host)
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

	if (!host.compare("/autoindex") || !host.compare("/"))
    	ss << "<p><a href=/"  << dirEntry + '>' + dirEntry + "</a></p>\n";
	else
   		ss << "<p><a href=" + host + "/" << dirEntry + '>' + dirEntry + "</a></p>\n";

    return ss.str();
}

std::string Http_handler::__base64_encode(const std::string &input)
{
	// Initialize a base64 alphabet
	const std::string base64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz"
										"0123456789+/";

	// Initialize the base64 string to be returned
	std::string base64_string;

	// Get the length of the input string
	size_t input_length = input.length();

	// Iterate through the input string in blocks of 3 characters
	for (size_t i = 0; i < input_length - 1; i += 3)
	{
		// Initialize a vector to store the 3 bytes
		std::vector<unsigned char> triplet(3);

		// Store the first, second and third character in the triplet
		triplet[0] = (i < input_length) ? input[i] : 0;
		triplet[1] = (i + 1 < input_length) ? input[i + 1] : 0;
		triplet[2] = (i + 2 < input_length) ? input[i + 2] : 0;

		// Convert the triplet to a 4-character base64 string
		base64_string += base64_alphabet[triplet[0] >> 2];
		base64_string += base64_alphabet[((triplet[0] & 0x03) << 4) | (triplet[1] >> 4)];
		base64_string += (i + 1 < input_length) ? base64_alphabet[((triplet[1] & 0x0f) << 2) | (triplet[2] >> 6)] : '=';
		base64_string += (i + 2 < input_length) ? base64_alphabet[triplet[2] & 0x3f] : '=';
	}

	// Handle remaining characters
	if (input_length % 3 == 1)
	{
		base64_string += base64_alphabet[input[input_length - 1] >> 2];
		base64_string += base64_alphabet[(input[input_length - 1] & 0x03) << 4];
		base64_string += "==";
	}
	else if (input_length % 3 == 2)
	{
		base64_string += base64_alphabet[input[input_length - 2] >> 2];
		base64_string += base64_alphabet[((input[input_length - 2] & 0x03) << 4) | (input[input_length - 1] >> 4)];
		base64_string += base64_alphabet[(input[input_length - 1] & 0x0f) << 2];
		base64_string += "=";
	}

	return base64_string;
}

std::string Http_handler::__base64_decode(const std::string &input)
{
	// Initialize a base64 alphabet
	const std::string base64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz"
										"0123456789+/";

	// Initialize the decoded string to be returned
	std::string decoded_string;
	decoded_string.resize((input.length() / 4) * 3);

	// Get the length of the input string
	size_t input_length = input.length();

	// Iterate through the input string in blocks of 4 characters
	for (size_t i = 0, j = 0; i < input_length; i += 4, j += 3)
	{
		// Initialize a vector to store the 4 base64 characters
		std::vector<unsigned char> quad(4);

		// Store the first, second, third and fourth character in the quad
		quad[0] = (i < input_length) ? base64_alphabet.find(input[i]) : 0;
		quad[1] = (i + 1 < input_length) ? base64_alphabet.find(input[i + 1]) : 0;
		quad[2] = (i + 2 < input_length) ? base64_alphabet.find(input[i + 2]) : 0;
		quad[3] = (i + 3 < input_length) ? base64_alphabet.find(input[i + 3]) : 0;

		// Check for padding characters
		bool padding_1 = (i + 1 < input_length && input[i + 1] == '=');
		bool padding_2 = (i + 2 < input_length && input[i + 2] == '=');
		bool padding_3 = (i + 3 < input_length && input[i + 3] == '=');

		// Convert the quad to a 3-character string
		decoded_string[j] = static_cast<char>((quad[0] << 2) + (quad[1] >> 4));
		decoded_string[j + 1] = (padding_1) ? '\0' : static_cast<char>(((quad[1] & 0x0f) << 4) + (quad[2] >> 2));
		decoded_string[j + 2] = (padding_2) ? '\0' : static_cast<char>(((quad[2] & 0x03) << 6) + quad[3]);
	}

	// Resize the decoded string to the correct length
	size_t padding = 0;
	if (input[input_length - 1] == '=')
		padding++;
	if (input[input_length - 2] == '=')
		padding++;
	decoded_string.resize(decoded_string.length() - padding);

	return decoded_string;
}

void	Http_handler::__Etag_gen(std::string path)
{
	// check path 404 if not found
	struct stat file_stat;
	if (stat(path.c_str(), &file_stat) != 0)
	{
		std::cout << path << std::endl;
		throw 404;
	}

	// get last time modified
	// to GMT
	struct tm	*gmt_time = gmtime(&file_stat.st_mtime);
	time_t		last_modified = mktime(gmt_time);

	char buffer[32];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", gmtime(&last_modified));

    std::string	file_to_encode(buffer);	
	file_to_encode += static_cast<char>(-1) + path;
	
	std::string	etag(__base64_encode(file_to_encode));
	this->header_ETag = "ETag: \""  + etag + "\"";
}

void	Http_handler::__Etag_reader(std::string &etag)
{
	if (etag.front() == '"')
		etag.erase(etag.begin());
	if (etag.back() == '"')
		etag.pop_back();

	etag = __base64_decode(etag);
	// find separator to get path and time.
	int	sep = etag.find(static_cast<char>(-1));
	// si ne trouve pas le séparateur alors ce n'est pas un ETag valid (not generated by us)
	if (sep == std::string::npos)
		throw 412;

	std::string	file = etag.substr(sep + 1);

	// si le path dans etag n'existe pas et que le etag est quand même reconnue
	// alors Etag obsolete
	struct stat file_stat;
	if (stat(file.c_str(), &file_stat) != 0)
		throw 412;
}

void	Http_handler::__condition_header(std::string address, Condition condition)
{
	struct stat	file_stat;
	if (stat(address.c_str(), &file_stat) != 0)
		throw 404;

	std::map<std::string, std::string>::iterator it;
	if (condition = MODIFIED_SINCE)
		it = this->_req_dict.find("If-Modified-Since");

	struct tm *gmt_time = gmtime(&file_stat.st_mtime);
	time_t	last_modified = mktime(gmt_time);
	time_t	request_time = __string_to_time_t_header(it->second);

	if (request_time && request_time >= last_modified)
		throw 304;
}

void Http_handler::__range_cut(std::string &range_val)
{
    int first = 0;
    int second = 0;

    // Check if the header starts with "bytes=", which indicates a byte range.
    if (range_val.find("bytes=") == 0)
    {
        // Split the header into the range values.
        std::string rangeValues = range_val.substr(6);
        std::stringstream ss(rangeValues);
        std::string startString, endString;
        std::getline(ss, startString, '-');
        std::getline(ss, endString);

        // Convert the start and end values to integers.
        std::stringstream startStream(startString);
        std::stringstream endStream(endString);

        startStream >> first;
        endStream >> second;
    }

    // Check that the start index is non-negative and the end index is greater than the start index.
	if (first > this->_response.length() - 1 || first < 0
		|| second > this->_response.length() - 1 || second < 0
		|| second < first)
		throw 416;

	// Extract the substring if the response string is non-empty.
	if (!this->_response.empty() && first )
		this->_response = this->_response.substr(first, second - first + 1);
}









// UTILS
void	Http_handler::__clean_address()
{
	size_t pos = this->_address.find("://");
	if (pos != std::string::npos)
	{
		this->_address = this->_address.substr(pos + 3);
		this->_address = this->_address.substr(this->_address.find('/'));
	}
	if (!this->_address.compare("/"))
	{
		int i = this->_address.length() - 1; 
		while (i > 0 && this->_address[i] == '/' && this->_address[i - 1] == '/')
		{
			this->_address.erase(i);
			i--;
		}
	}
	
	std::stringstream ss;
	for (std::string::iterator it = this->_address.begin(); it != this->_address.end(); ++it)
	{
		if (*it == '%')
		{
			// This character is the start of a URL-encoded sequence.
			// Get the next two characters and interpret them as a hexadecimal number.
			std::string hex_str = "0x" + std::string(it + 1, it + 3);
			int value;
			ss << std::hex << hex_str;
			ss >> value;
			// Replace the URL-encoded sequence with the corresponding character.
			*it = static_cast<char>(value);
			// Erase the next two characters from the string.
			this->_address.erase(it + 1, it + 3);
			// Increment the iterator past the two characters that were erased.
			it += 2;
			// Clear the error flags on the stringstream, but don't reset it.
			ss.clear();
		}
	}
}



void	Http_handler::__close_and_throw(std::fstream &file, int err)
{
	file.close();
	throw err;
}

bool	Http_handler::__not_a_method()
{
	if (this->_method != "GET"
		&& this->_method != "POST"
		&& this->_method != "DELETE"
		&& this->_method != "HEAD"
		&& this->_method != "PUT"
		&& this->_method != "CONNECT"
		&& this->_method != "OPTIONS"
		&& this->_method != "TRACE"
		&& this->_method != "PATCH")
		return (true);

	return (false);
}

std::string	Http_handler::__get_extension()
{
	std::string	ext = this->_address.substr(this->_address.find_last_of(".") + 1);
	if (ext == this->_address)
		return (std::string());

	return (ext);
}

bool Http_handler::__is_valid_http_time(const std::string &s)
{
	static const char *date_format = "%a, %d %b %Y %H:%M:%S %Z";

	struct tm tm;
	std::memset(&tm, 0, sizeof(tm));

	if (strptime(s.c_str(), date_format, &tm) == nullptr)
		return false;

	// Check if the resulting time is valid
	time_t t = mktime(&tm);
	if (t == -1)
		return false;

	return true;
}

time_t	Http_handler::__string_to_time_t_header(const std::string &s)
{
	static const char *date_format = "%a, %d %b %Y %H:%M:%S %Z";

	struct tm tm;
	std::memset(&tm, 0, sizeof(tm));

	strptime(s.c_str(), date_format, &tm);
//		throw 400;
	return mktime(&tm);
}











std::string	Http_handler::get_time()
{
	time_t		rawtime;
	struct tm	*timeinfo;
	
	char buffer[80];

	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeinfo);

	return (std::string(buffer));
}

std::string	Http_handler::http_408()
{
	std::string	response;
	std::string header;


	response += "<!DOCTYPE html>\r\n";
	response += "<html>\r\n";
	response += "	<head>\r\n";
	response += "		<meta charset=\"UTF-8\">\r\n";
	response += "		<title>webserv</title>\r\n";
	response += "	</head>\r\n";
	response += "	<body bgcolor=\"white\">\r\n";
	response += "		<center><h1>" + std::to_string(408) + " " + g_errs[408] + "</h1></center>\r\n";
	response += "		<center><hr>Server: FT_Webserv/1.0.0</hr></center>\r\n";
	response +=			g_gif[1] + "\r\n";
	response += "		<center><p>Click <a href=\"/\">here</a> to return home.</p></center>\r\n";
	response += "	</body>\r\n";
	response += "</html>\r\n";

	header += "HTTP/1.1 " + std::to_string(408) + " " + g_errs[408] + "\r\n";
	header += "Content-Length: " + std::to_string(response.length() + 2) + "\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Date: " + get_time() + "\r\n";
	header += "Connection: close\r\n";

	response = header + "\r\n" + response + "\r\n";

	return (response);
}

//	one line by request
//	IP			auth	date						request						resp size		referer							user-agent------------------->
// 127.0.0.1 - frank [10/Oct/2000:13:55:36 -0700] "GET /apache_pb.gif HTTP/1.0" 200 2326 "http://www.example.com/start.html" "Mozilla/4.08 [en] (Win98; I ;Nav)"

