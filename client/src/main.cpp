#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int	create_socket()
{
	int	sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cerr << "Error socket\n";
		std::exit(1);
	}
	return (sock);
}


int	main(int ac, char **av)
{
	int	sock;

	sock = create_socket();
	#include <fcntl.h>




	std::string host;
	std::string	port;
	if (ac >= 2) host = av[1];
	else host = "localhost";
	if (ac >= 3) port = av[2];
	else port = "8080";

	struct addrinfo	hints;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	struct addrinfo	*addrinfo;
	int res = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrinfo);
	if (res < 0) {
		std::cerr << "Error addinfo\n" << gai_strerror(res) << "\n";
		std::exit(1);
	}


	res = connect(sock, addrinfo->ai_addr, addrinfo->ai_addrlen);
	if (res < 0) {
		std::cerr << "Error connect\n";
		std::exit(1);
	}


	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	while (true) {
		std::string msg;

		std::getline(std::cin, msg);
		msg += "\r\n";
		send(sock, msg.c_str(), msg.length(), 0);

		char	rec[1024];
		int br = recv(sock, rec, 1023, 0);
		if (!br)
			std::cout << "server closed the connection\n";
		if (br > 0) {
			rec[br] = 0;
			if (br)
				std::cout << rec << '\n';
		}
		if (br == -1)
			std::cout << "we got minus one!\n";
	}



	close(sock);
	return (0);
}
