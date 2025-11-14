#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <iostream>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

std::string response = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 637\r\nCache-Control: s-maxage=300, public, max-age=0\r\nContent-Language: en-US\r\nDate: Thu, 06 Dec 2018 17:37:18 GMT\r\nETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"\r\nServer: meinheld/0.6.1\r\nStrict-Transport-Security: max-age=63072000\r\nX-Content-Type-Options: nosniff\r\nX-Frame-Options: DENY\r\nX-XSS-Protection: 1; mode=block\r\nVary: Accept-Encoding,Cookie\r\nAge: 7\r\n\r\n<!doctype html>\r\n<html lang=\"en\">\r\n<head>\r\n<meta charset=\"utf-8\">\r\n<title>A basic webpage</title>\r\n</head>\r\n<body>\r\n<h1>Basic HTML webpage</h1>\r\n<p>Hello, world!</p>\r\n</body>\r\n</html>\r\n";
std::string response2 = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 637\r\nCache-Control: s-maxage=300, public, max-age=0\r\nContent-Language: en-US\r\nDate: Thu, 06 Dec 2018 17:37:18 GMT\r\nETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"\r\nServer: meinheld/0.6.1\r\nStrict-Transport-Security: max-age=63072000\r\nX-Content-Type-Options: nosniff\r\nX-Frame-Options: DENY\r\nX-XSS-Protection: 1; mode=block\r\nVary: Accept-Encoding,Cookie\r\nAge: 7\r\n\r\n<!doctype html>\r\n<html lang=\"en\">\r\n<head>\r\n<meta charset=\"utf-8\">\r\n<title>A basic webpage</title>\r\n</head>\r\n<body>\r\n<h1>Basic HTML webpage</h1>\r\n<p>Hello, you!</p>\r\n</body>\r\n</html>\r\n";

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

	int	n = 0;

	while (1) {
		int	client_fd = 0;	
		struct sockaddr_in	client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (client_fd == -1)
			std::cout << "Error\n";
		if (client_fd > 0) {
			std::cout << client_fd << std::endl;

			ret = 0;
			if (n) {
				if ((ret = send(client_fd, (const void *) response.c_str(), response.length(), 0)) < 0) {
					std::cout << "ERROR4\n";
				}
				--n;
			}
			else {
				if ((ret = send(client_fd, (const void *) response2.c_str(), response.length(), 0)) < 0) {
					std::cout << "ERROR5\n";
				}
				n++;
			}


		}
	}
	return (0);
}
