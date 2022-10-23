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
			If not good number of argument
				- Error message is displayed and the usage of the executable is mentioned
				The execution stops here
		Trying to open the file.
		If output stream is set to failbit, the exception it throwed
			Execution stops here.
	*/
	if (ac == 1)
		fatal_error("Not enough argument, usage: webserv <path_to_config_file>", 1);
	if (ac != 1)
		fatal_error("Too many arguments, usage: webserv <path_to_config_file>", 1);
	// std::ofstream config_file(av[1]);
	// try
	// {
	// 	config_file.exceptions(config_file.failbit);
	// }
	// catch(const std::ios_base::failure& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	fatal_error("unable to pursue the execution", 2);
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	fatal_error("unable to pursue the execution", 2);
	// }
	
	// Conf	config(config_file);
//	Webserv test(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
	Webserv test;

	test.launch();
//	listen_ft();
	return 0;
}

