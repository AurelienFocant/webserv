#include <stdlib.h>
#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <poll.h>
#include <sys/epoll.h>
#include <map>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#define MAX_EVENTS 1024

#include "Webserv.hpp"


int	main(int ac, char **av)
{
	if (ac > 2) {
		std::cerr << "Usage: ./webserv [config_file]\n";
		return (1);
	}


	Webserv	webserv(av[1]);
	try {
		webserv.readConfig();
		webserv.initWebServer();
		webserv.run();
	}
	catch (std::exception &e) {
		std::cerr << "Exception happened: " << e.what() << std::endl;
		return (2);
	}

	return (0);
}
