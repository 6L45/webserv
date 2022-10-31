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
	
	if (ac == 1)
		fatal_error("Not enough argument, usage: webserv <path_to_config_file>", 1);
	if (ac != 2)
		fatal_error("Too many arguments, usage: webserv <path_to_config_file>", 1);
	std::string	path_to_conf(av[1]);
	
		Check if the extention of configuration file is valid
		File form : *.conf
		note : better to put this in config structure
	
	if (path_to_conf.rfind(".conf") == std::string::npos
		|| path_to_conf.compare(path_to_conf.rfind(".conf"), std::string::npos, ".conf"))
		fatal_error("Unvalid extention of config file", 2);
	
		Opening the configuration file in read_only
		note : better to put this in config structure
	
	std::ifstream config_file;
	config_file.open(path_to_conf);
	if (config_file.fail())
		fatal_error("unable to open the configuration file", 2);
	
	Conf	config(config_file);
*/
	Webserv test;

	test.launch();
//	listen_ft();
	return 0;
}
