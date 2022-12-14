#include "conf.class.hpp"
#include <type_traits>
#include <string>

Conf::Conf()
{ } //nothing here, unauthorised constructor


Conf::Conf(std::string path_to_file)
	:	_max_connexion(1024),
		_line_read(0)
{
	std::ifstream fs;

	if (! __valide_conf_file_name(path_to_file))
		throw std::invalid_argument("Unvalid extention of configuration file");
	fs.open(path_to_file);
	fs.exceptions(std::ifstream::failbit); //may throw
	__parse_config_file(fs);
	fs.close();
	__print_everything();
}

/*
	It's pretty clear what this function is up for
*/
void	Conf::__parse_config_file(std::ifstream& fs)
{
	std::string line;

	while (!fs.eof())
	{
		__get_line(fs, line);
		__erase_comment(line);
		__erase_tab_space(line);
		if (line.empty())
			continue;
		if (!line.compare(0, 14, "max-connexion:"))
			__add_to<int>(this->_max_connexion, line.erase(0, line.find(':') + 1));
		else if (!line.compare(0, 23, "default-error-log-root:"))
			__add_to<std::string>(this->_default_error_root, line.erase(0, line.find(':') + 1));
		else if (!line.compare(0, 6, "server"))
			__parse_server(fs, line.erase(0,6));
		else
			__error_file_notif(_line_read, "unknown parameter");
	}
}

Conf::~Conf()
{
	
}

void	Conf::__parse_server(std::ifstream &fs, std::string& line)
{
	__server_conf new_serv;

	new_serv.methods = 0;
	new_serv.options = 0;
	__erase_tab_space(line);
	if (line.empty() && !fs.eof())
		__get_line(fs, line);
	__erase_tab_space(line);
	if (line.empty() || line.front() != '{')
		return (__error_file_notif(_line_read, "missing '{'"));
	while (!fs.eof())
	{
		__get_line(fs, line);
		__erase_comment(line);
		__erase_tab_space(line);
		if (line.empty())
			continue;
		if (!line.compare(0, 12, "server-name:"))
			__add_to<std::string>(new_serv.server_name, line.erase(0, line.find(':') + 1));
		else if (!line.compare(0, 5, "port:"))
			__add_to<int>(new_serv.port, line.erase(0, line.find(':') + 1));
		else if (!line.compare(0, 5, "host:"))
		{
			line.erase(0, line.find(':') + 1);
			__erase_tab_space(line);
			while (!line.empty())
			{
				std::string temp;
				__get_info(temp, line);
				new_serv.host.push_back(temp);
				__erase_tab_space(line);
			}
		}
		else if (!line.compare(0, 5, "root:"))
			__add_to<std::string>(new_serv.root, line.erase(0, line.find(':') + 1));
		else if (!line.compare(0, 19, "unactive-max-delay:"))
			__add_to<int>(new_serv.unactive_max_delay, line.erase(0, line.find(':') + 1));
		else if (!line.compare(0, 18, "directory-browser:"))
		{
			//check the argument if == activate
			_SC_DIRACTIVATE(new_serv);
		}
		else if (!line.compare(0, 4, "html"))
			__parse_html(fs, line.erase(0,4), new_serv);
		else if (!line.compare(0, 1, "}"))
			break ;
		else
			__error_file_notif(_line_read, "unknown parameter");
	}
	if (fs.eof() && line.compare(0, 1, "}"))
		return (__error_file_notif(_line_read, "missing '}' for server"));
	line.erase(0,1);
	if (!line.empty())
		__error_file_notif(_line_read, "line ignored after '}'");
	_sc.push_back(new_serv);
}

void	Conf::__parse_html(std::ifstream &fs, std::string& line, __server_conf& srv)
{
	__erase_tab_space(line);
	if (line.empty() && !fs.eof())
		__get_line(fs, line);
	__erase_tab_space(line);
	if (line.empty() || line.front() != '{')
		return (__error_file_notif(_line_read, "missing '{'"));
	while (!fs.eof())
	{
		__get_line(fs, line);
		__erase_comment(line);
		__erase_tab_space(line);
		if (line.empty())
			continue;
		else if (!line.compare(0, 8, "methods:"))
		{
			line.erase(0, line.find(':') + 1);
			__erase_tab_space(line);
			while (!line.empty())
			{
				if (!line.compare(0, 3, "GET"))
					srv.methods = (srv.methods | (1 << 0));
				else if (!line.compare(0, 4, "POST"))
					srv.methods = (srv.methods | (1 << 1));
				else if (!line.compare(0, 6, "DELETE"))
					srv.methods = (srv.methods | (1 << 2));
				else
					__error_file_notif(_line_read, "unknown method");
				__erase_word(line);
				__erase_tab_space(line);
			}
		}
		else if (!line.compare(0, 6, "index:"))
		{
			line.erase(0, line.find(':') + 1);
			__erase_tab_space(line);
			while (!line.empty())
			{
				std::string temp;
				__get_info(temp, line);
				srv.index.push_back(temp);
				__erase_tab_space(line);
			}
		}
		else if (!line.compare(0, 16, "body-size-limit:"))
		{
			__add_to<int>(srv.body_limits, line.erase(0, line.find(':') + 1));
			_SC_BODYLIMITACTIVATE(srv);
		}
		else if (!line.compare(0, 1, "}"))
			break ;
		else
			__error_file_notif(_line_read, "unknown parameter");
	}
}

template<class T>
void	Conf::__add_to(T& to, std::string& s)
{
	T temp;

	// Checking if there is an argument
	__erase_tab_space(s);
	if (s.empty())
		return (__error_file_notif(_line_read, "parameter without arguments"));
	//Extract the info, may throw
	try
	{
		__get_info(temp, s); //petit probleme si int plus grand que le nombre de port max
	}
	catch (std::invalid_argument e)
	{
		return (__error_file_notif(_line_read, e.what()));
	}
	catch (std::out_of_range e)
	{
		return (__error_file_notif(_line_read, e.what()));
	}
	catch (std::exception e)
	{
		return (__error_file_notif(_line_read, e.what()));
	}
	// Checking if the good number of argument is given
	__erase_tab_space(s);
	if (!s.empty())
		return (__error_file_notif(_line_read, "too much arguments"));
	to = temp;
}

/* 
	Transform the info into the int pointed by c
	the used caracter are erased from input string
*/
void	Conf::__get_info(int &c, std::string& raw)
{
	size_t	size;

	c = std::stoi(raw, &size); //may throw (when int is bigger than INT_MAX)
	raw.erase(0, size);
}

/* 
	Extract the info in string type
	the used caracters are erased from input string
*/
void	Conf::__get_info(std::string &c, std::string& raw)
{
	c = raw;
	__erase_from_char(__erase_from_char(c, ' '), '\t');
	raw.erase(0, c.length());
}

/*
	Print in stderr the error message and the line of the config file where the error occures
*/
void	Conf::__error_file_notif(const int line, const std::string& error) const
{
	std::cerr << "line " << line << ": " << error << std::endl;
}

/*
	Erasing all the comment section of the line
*/
std::string&	Conf::__erase_comment(std::string& s) const
{
	return (__erase_from_char(s, '#'));
}

/*
	Erasing all the string after the caracter c
*/
std::string&	Conf::__erase_from_char(std::string& s, char c) const
{
	return (s.erase(std::min(s.find(c), s.length()), std::string::npos));
}

/*
	Erasing all the space and tab at the begginig of the line
*/
std::string::iterator	Conf::__erase_tab_space(std::string& s) const
{
	std::string::iterator it;

	for (it = s.begin(); it != s.end(); it++)
			if (*it != ' ' && *it != '\t')
				break;
	return (s.erase(s.begin(), it)); //what happens if erase(begin(),begin())
}

/*
	Erasing the first word of the line
*/
std::string&	Conf::__erase_word(std::string& s) const
{
	int f_space = s.find(' ');
	int f_tab = s.find('\t');

	int f_erase = f_space == -1 ? f_tab : f_space;
	f_erase = f_tab == -1 ? f_erase : std::min(f_erase, f_tab); // all four condiitions are treated

	s.erase(0,f_erase);
	return (s);
}

/* Same as getline but incrementing current line index */
void	Conf::__get_line(std::ifstream &fs, std::string& line)
{
	_line_read++;
	std::getline(fs, line);
}

/*
	Verbose printing all the info we get from the configuration file
*/
void	Conf::__print_everything() const
{
	std::cout << "Configuration file parsing" << std::endl;
	std::cout << "Max connexion authorised - " << this->_max_connexion << std::endl;
	if (this->_default_error_root.empty())
		std::cout << "No default root for log error initialised" << std::endl;
	else
		std::cout << "Default root for log errors - " << this->_default_error_root << std::endl;
	for (std::vector<__server_conf>::const_iterator it = _sc.begin(); it != _sc.end(); it++)
	{
		std::cout << "Server : " << it->server_name << std::endl;
		std::cout << "	Port - " << it->port << std::endl;
		std::cout << "	Host - ";
		for (std::vector<std::string>::const_iterator c_it = it->host.begin(); c_it != it->host.end(); c_it++)
			std::cout << *c_it << " ";
		std::cout << std::endl;
		std::cout << "	Index - ";
		for (std::vector<std::string>::const_iterator c_it = it->index.begin(); c_it != it->index.end(); c_it++)
			std::cout << *c_it << " ";
		std::cout << std::endl;
 		std::cout << "	Root - " << it->root << std::endl;
		if (SC_BODYISLIMITED((*it)))
			std::cout << "	Body limit size - " << it->body_limits << std::endl;
		if (!SC_DIRISACTIVE((*it)))
			std::cout << "	The Directory browsing is desactivated" << std::endl;
		std::cout << "	available methods : ";
		if (it->methods & (1 << 0))
			std::cout << "GET ";
		if (it->methods & (1 << 1))
			std::cout << "POST ";
		if (it->methods & (1 << 2))
			std::cout << "DELETE ";
		if (!it->methods)
			std::cout << "none";
		std::cout << std::endl;
	}

	std::cout << "End of Conifguration file parsing" << std::endl;
}

/*	
		Check if the extention of configuration file is valid
		File form : *.conf (defined with MACRO  CONF_EXTENTION_S_NAME in case of changing)
		note : better to put this in config structure
*/	
bool	Conf::__valide_conf_file_name(const std::string& file_name) const
{
	return (!(file_name.rfind(CONF_EXTENTION_S_NAME) == std::string::npos
		|| file_name.compare(file_name.rfind(CONF_EXTENTION_S_NAME), std::string::npos, CONF_EXTENTION_S_NAME)));
}
