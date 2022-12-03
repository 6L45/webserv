/*
HTTP/1.1 200 OK
Content-Length: 30
Content-Location: /index.html
Content-Type: text/html
Date: Sat, 03 Dec 2022 08:50:43 GMT
Last-Modified: Sat, 03 Dec 2022 08:47:23 GMT
Server: Weebserv/1.0.0 (Unix)
Transfer-Encoding: identity
*/

#include <iostream>
#include <ctime>

int main ()
{
	time_t		rawtime;
	struct tm	*timeinfo;
	
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string str(buffer);

	std::cout << str << std::endl;

	return 0;
}