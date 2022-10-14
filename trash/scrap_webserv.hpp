#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "common.hpp"

class Server
{
	public :
		Server()
		{
			if ( (this->_listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
			{
				std::cout << "socket creation failure" << std::endl;
				exit(EXIT_FAILURE);
			}
			bzero(&(this->_servaddr), sizeof(this->_servaddr));
			this->_servaddr.sin_family = AF_INET;
			this->_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			this->_servaddr.sin_port = htons(SERVER_PORT);

			if ( (bind(this->_listenfd, reinterpret_cast<SA *>(&(this->_servaddr)), sizeof(this->_servaddr) )) < 0 )
			{
				std::cout << "bind failure" << std::endl;
				exit(EXIT_FAILURE);
			}
			if ( (listen(this->_listenfd, 10)) < 0 )
			{
				std::cout << "listen failure" << std::endl;
				exit(EXIT_FAILURE);
			}
			std::cout << "so far so good" << std::endl;
		};
		~Server(){};

		void	launch()
		{
			int n;
			while(1)
			{
				std::cout << "la" << std::endl;
				struct sockaddr_in addr;
				socklen_t addr_len;

				std::cout << "waiting for a connection on port" << SERVER_PORT << std::endl;
				fflush(stdout);
				this->_connfd = accept(this->_listenfd, (SA *) NULL, NULL);
				//memset(this->_recvline, 0, MAXLINE);
				while ( (n = read(this->_connfd, this->_recvline, MAXLINE - 1)) > 0 )
				{
					std::cout << std::endl << this->_recvline << std::endl << std::endl << this->_recvline << std::endl;
					if (this->_recvline[n - 1] == '\n')
					{
						break;
					}
				}
				if (n < 0)
					std::cout << "read error" << std::endl;
				snprintf( (char *)this->_buff, sizeof(this->_buff), "HTTP/1.0 200 OK\r\n\r\nHello" );
				write(this->_connfd, (char *)_buff, strlen((char *)_buff));
				close(this->_connfd);
			}
		}

	private :

		struct sockaddr_in	_servaddr;
		int			_listenfd;
		int			_connfd;
		char		_buff[MAXLINE + 1];
		char		*_recvline;
};

/*
class Webserver
{
	public :
		Webserver(){};
		~Webserver(){};

		init();
		launch();
	private :
		
};
*/

#endif

