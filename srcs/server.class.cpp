#include "server.class.hpp"

Server::Server(__server_conf sc, char **env)
	:	_port(sc.port),
		_host(sc.host),
		_index(sc.index),
		_root(sc.root),
		_name(sc.server_name),
		_env(env),
		_location(sc.location)
{ }

// bool		Server::cgi_exec(const std::string& file_name) const
// {
// 	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = _cgi.begin(); it != _cgi.end(); it++)
// 	{
// 		if (it->first.size() > file_name.size())
// 			continue ;
// 		if (! file_name.compare(file_name.size() - it->first.size(), it->first.size(), it->first))
// 			return (true);
// 	}
// 	return (false);
// }

// std::string		Server::get_cgi_path(const std::string& file_name) const
// {
// 	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = _cgi.begin(); it != _cgi.end(); it++)
// 	{
// 		if (it->first.size() > file_name.size())
// 			continue ;
// 		if (! file_name.compare(file_name.size() - it->first.size(), it->first.size(), it->first))
// 			return (it->second);
// 	}
// 	return (NULL);
// }

bool		Server::belong_to(const std::string& host_port) const
{
	std::string s_host(host_port, 0, host_port.find(':'));
	std::string s_port(host_port, host_port.find(':') + 1); //no except safe

	if (s_port.compare(std::to_string(_port)))
		return (false);
	for (std::vector<std::string>::const_iterator c_it = _host.begin(); c_it != _host.end(); c_it++)
	{
		if (! (*c_it).compare(s_host))
			return (true);
	}

	return (false);
}