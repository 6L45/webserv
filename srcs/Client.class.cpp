#include "Client.class.hpp"

Client::Client(int fd, int port, int timer)
	:	_fd(fd),
		_port(port),
		_activity_timer(timer)
{ }