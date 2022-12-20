#pragma once

# include <sys/types.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <dirent.h>

# include <map>
# include <unordered_map>
# include <vector>
# include <list>
# include <fstream>
# include <cstring>
# include <iostream>
# include <csignal>
# include <algorithm>
# include <sstream>
# include <utility>
# include <stdexcept>
# include <fstream>
# include <filesystem>
# include <ctime>
# include <utility>
# include <cstdio>
# include <cstdlib>  // for rand(), srand(), and RAND_MAX

# include <unistd.h>
# include <sys/wait.h>

# include "http_ret_code.hpp"

# define SERVER_PORT 18002
# define SA struct sockaddr
# define MAXLINE 4096
# define PORT 18005
# define CONF_EXTENTION_S_NAME ".conf"
