#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <stdlib.h>
#include <iostream>
#include <arpa/inet.h>

void	print_ai_structs(struct addrinfo *addrinfo)
{
	while (addrinfo)
	{
		std::cerr
			<< "int	ai_flags:	"		<< addrinfo->ai_flags		<< '\n'
			<< "int	ai_family:	"		<< addrinfo->ai_family		<< '\n'
			<< "int	ai_socktype:	"	<< addrinfo->ai_socktype	<< '\n'
			<< "int	ai_protocol:	"	<< addrinfo->ai_protocol	<< '\n'
			<< "(int)	ai_addrlen:	"	<< addrinfo->ai_addrlen		<< '\n';
			if (addrinfo->ai_canonname) {
				std::cerr
					<< "char*	ai_canonname:"	<< addrinfo->ai_canonname << '\n';
			}
			else {
				std::cerr
					<< "there is no canonnical name\n";
			}

		if (addrinfo->ai_family == AF_INET) {
			sockaddr_in	*ipv4;
			char		ipstr[INET_ADDRSTRLEN];

			ipv4 = (struct sockaddr_in *) addrinfo->ai_addr;
			inet_ntop(addrinfo->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));

			std::cerr << "The address is IPv4:\n";
			std::cerr
				<< "sin_family:	"	<< ipv4->sin_family			<< '\n'
				<< "sin_port:	"	<< ntohs(ipv4->sin_port)	<< '\n'
				<< "sin_addr:	"	<< ipstr					<< '\n';
		}
		else if (addrinfo->ai_family == AF_INET6) {
			sockaddr_in6	*ipv6;
			char			ipstr[INET6_ADDRSTRLEN];

			ipv6 = (struct sockaddr_in6 *) addrinfo->ai_addr;
			inet_ntop(addrinfo->ai_family, &(ipv6->sin6_addr), ipstr, sizeof(ipstr));

			std::cerr << "The address is IPv6:\n";
			std::cerr
				<< "sin_family:	"	<< ipv6->sin6_family		<< '\n'
				<< "sin_port:	"	<< ntohs(ipv6->sin6_port)	<< '\n'
				<< "sin_addr:	"	<< ipstr					<< '\n';
		}

		std::cerr << std::endl;
		addrinfo = addrinfo->ai_next;
	}
}

int	main(int ac, char **av)
{
	if (ac > 2) {
		std::cerr << "Usage: ./webserv [config_file]";
		(void) av;
		return (1);
	}


	struct addrinfo	hints;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE;


	int ret;
	struct addrinfo	*res;
	if ((ret = getaddrinfo(NULL, "http", &hints, &res)) != 0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(ret);
	}

	print_ai_structs(res);

	int	listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	(void) listenfd;



	freeaddrinfo(res);
	return (EXIT_SUCCESS);
}
