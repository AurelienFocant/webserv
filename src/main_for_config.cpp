#include <iostream>
#include <cstdlib>
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
	}
	catch (std::exception &e) {
		std::cerr << "Exception caught: " << e.what() << '\n';
		return (2);
	}
}
