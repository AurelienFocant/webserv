#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <iostream>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

int	main()
{
	int	sockfd;
	int ret;
	struct sockaddr_in server_addr;
	// std::string buffer;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8080);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	ret = bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (ret)
		std::cout << "ERROR2\n";

	ret = listen(sockfd, 256);
	if (ret)
		std::cout << "ERROR3\n";


	while (1) {
		int	client_fd = 0;	
		struct sockaddr_in	client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (client_fd == -1)
			std::cout << "Error\n";
		if (client_fd > 0) {
			std::cout << client_fd << std::endl;

			char buffer[] =  "HTTP/1.1 200 OK\r\nServer: webserv\r\n\r\n<!DOCTYPE html>\r\n<html> hello world </html>";
			ret = 0;

			if ((ret = send(client_fd, buffer, strlen(buffer), 0)) < 0) {
				std::cout << "ERROR4\n";
			}

		close(client_fd);

		}
	}




	return (0);
}
