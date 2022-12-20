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
	if (ac == 1)
		fatal_error("Not enough argument, usage: webserv <path_to_config_file>", 1);
	if (ac != 2)
		fatal_error("Too many arguments, usage: webserv <path_to_config_file>", 1);
	std::string	path_to_conf(av[1]);
	try
	{
		Conf	config(path_to_conf);
		Webserv test(config);
		test.launch();
	}
	catch(const std::exception& e)
	{
		fatal_error(e.what(), 1);
		return 1;
	}
//	listen_ft();
	return 0;
}
