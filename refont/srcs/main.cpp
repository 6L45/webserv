#include "webserv.hpp"

int guard(int n) { if (n == -1) { exit(1); } return n; }

int main(int ac, char **av, char **env)
{
//	Webserv test(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
	Webserv test;

	test.launch();
//	listen_ft();
	return 0;
}

