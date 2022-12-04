#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <vector>

int		g_port;
int		g_sock;
bool	g_connected;

void	fatal_error(std::string msg, int exit_status)
{
	std::cerr << "client_simulator: " << msg << std::endl;
	exit(exit_status);
}

void _update_port(const std::string& str)
{
	g_port = std::stoi(str);
	std::cout << "Port Updates. New port for connexion : " << g_port << std::endl;
}

void _send_message(const std::string& msg)
{
	int msg_size;

	if (g_connected)
	{
		msg_size = send(g_sock, msg.c_str(), msg.length(), 0);
		std::cout << "Message send to server. Msg length " << msg_size << std::endl;
		return ;
	}
	std::cout << "No connexion established, nowhere to send" << std::endl;
}

void _chill(const std::string& str)
{
	unsigned int	u_time = std::stoi(str);

	std::cout << "Client need to rest " << u_time << "miliseconds" << std::endl;
	usleep(u_time);
}

void _close_connexion()
{
	if (!g_connected)
	{
		std::cout << "Connexion doesn't exist, nothing to close" << std::endl;
		return ;
	}
	close(g_sock);
	std::cout << "Connexion closed with the server" << std::endl;
	g_sock = -1;
	g_connected = false;
}

void _receave_text()
{
	std::vector<char> buf(5000);
	int bytes_recv;
	
	if (g_connected)
	{
		bytes_recv = recv(g_sock, buf.data(), buf.size(), 0);
		std::cout << "Message received from server, for a total of " << bytes_recv << "bytes" << std::endl;
		std::cout << "Message content ->" << std::endl;
		std::cout << "************************************************************" << std::endl;
		std::cout << buf.data() << std::endl;
		std::cout << "****************END OF SERVER MESSAGE **********************" << std::endl;
		return ;
	}
	std::cout << "No connexion established, listening to the void" << std::endl;
}

void _connexion_request()
{
	struct sockaddr_in sa;

	if (g_connected)
	{
		std::cout << "Connexion already existe, calling the closing connexion function" << std::endl;
		_close_connexion();
	}
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == -1)
		fatal_error("unable to create the socket", 3);
	std::cout << "Socket created : " << g_sock << std::endl;
	memset(&sa, 0, sizeof(sa)); // here the adress is filled with 0, menaning connexion to localhost?
	sa.sin_family = AF_INET;
	sa.sin_port = htons(g_port);
	if (connect(g_sock, (struct sockaddr *)&sa, (socklen_t)sizeof(sa)) == -1)
	{
		std::cout << "failed to connect, something went wrong" << std::endl;
		return ;
	}
	std::cout << "Connexion to the webserver is successefull" << std::endl;
	g_connected = true;
}


int main(int ac, char **av)
{
	g_port = -1;
	g_sock = -1;
	g_connected = false;

	if (ac != 2)
		fatal_error("usage: webserv process_files", 1);
	std::string file_path(av[1]);
	std::ifstream fs;
	fs.open(file_path);
	if (!fs.is_open())
		exit(1);
	
	std::string line;
	while (!fs.eof())
	{
		std::getline(fs, line);
		if (line.empty())
			continue ;
		if (!line.compare(0,4,"send"))
			_send_message(line.erase(0,4));
		else if ((!line.compare(0,4,"port")))
			_update_port(line.erase(0,4));
		else if (!line.compare(0,4,"wait"))
			_chill(line.erase(0,4));
		else if (!line.compare(0,5, "close"))
			_close_connexion();
		else if (!line.compare(0,4,"recv"))
			_receave_text();
		else if (!line.compare(0,7,"connect"))
			_connexion_request();
		else
			std::cout << "unknown commande, line ignored" << std::endl;
	}
	fs.close();
	if (g_sock != -1)
		close(g_sock);
	return (0);
}
