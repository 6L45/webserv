#include "webserv.class.hpp"
#include "conf.class.hpp"

int guard(int n) { if (n == -1) { exit(1); } return n; }

void	fatal_error(std::string msg, int exit_status)
{
	std::cerr << "webserv: " << msg << std::endl;
	exit(exit_status);
}

int main(int ac, char **av, char **env)
{
/*
		Check if there is a good number of argument
		Usage : webserv <path_to_config_file>
*/	

    srand(time(nullptr));

#ifndef CONF_BYPASS
	if (ac == 1)
		fatal_error("Not enough argument, usage: webserv <path_to_config_file>", 1);
	if (ac != 2)
		fatal_error("Too many arguments, usage: webserv <path_to_config_file>", 1);
	std::string	path_to_conf(av[1]);
	try
	{
		Conf	config(path_to_conf);
		Webserv webserver(config, env);
		webserver.launch();
	}
	catch(const std::exception& e)
	{
		fatal_error(e.what(), 1);
		return 1;
	}
#else
	try
	{
		__server_conf	bypass_config;

		bypass_config.server_name = "localhost";
		bypass_config.port = 18000;
		bypass_config.root = "./www/web/serveur1/";
		bypass_config.host.push_back("localhost");
		bypass_config.index.push_back("index.php");
		bypass_config.index.push_back("index.html");
		bypass_config.index.push_back("index.htm");
		bypass_config.cgi.push_back(std::make_pair(".py", "/usr/bin/python3")) ;
		bypass_config.cgi.push_back(std::make_pair(".php", "/usr/bin/php")) ;
		bypass_config.options = CHAR_MAX;
		bypass_config.methods = CHAR_MAX;
		bypass_config.body_limits = 0;
		bypass_config.body_min_size = 0;
		bypass_config.unactive_max_delay = 0;

		Conf	confception;

		confception._sc.push_back(bypass_config);

		Webserv	webserver(confception, env);
		webserver.launch();
	}
	catch(const	std::exception &e)
	{
		fatal_error(e.what(), 1);
		return 1;
	}
#endif

//	listen_ft();
	return 0;
}
