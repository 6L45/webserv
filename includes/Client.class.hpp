#ifndef CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP

class Client
{
public:
	Client(int fd, int port, int timer);
private:
	int		_fd;
	int		_port;
	int		_activity_timer;

	friend class Webserv;
};

#endif