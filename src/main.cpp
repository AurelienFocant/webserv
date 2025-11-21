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

#include <vector>
#include <map>

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>

#define MAX_EVENTS 1024

class Connection
{
	public:
		int					clientFd;
		struct sockaddr_in	clientAddr;
		std::string			request;
		int					requOffset;
		std::string			response;
		int					respOffset;

		void	receiveRequest() {
		};
		void	parseRequest() {
		};
		void	sendResponse() {
		};
};

std::string	build_response(void)
{
	std::string header =
		"HTTP/1.1 200 OK\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Content-Length: 185\r\n"
		"Cache-Control: s-maxage=300, public, max-age=0\r\n"
		"Content-Language: en-US\r\n"
		"Date: Thu, 06 Dec 2018 17:37:18 GMT\r\n"
		"ETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"\r\n"
		"Server: meinheld/0.6.1\r\n"
		"Strict-Transport-Security: max-age=63072000\r\n"
		"X-Content-Type-Options: nosniff\r\n"
		"X-Frame-Options: DENY\r\n"
		"X-XSS-Protection: 1; mode=block\r\n"
		"Vary: Accept-Encoding,Cookie\r\n"
		"Age: 7\r\n"
		"\r\n";

	std::string body =
		"<!doctype html>\r\n"
		"<html lang=\"en\""
		">\r\n"
		"<head>\r\n"
		"<meta charset=\"utf-8\""
		">\r\n"
		"<title>A basic webpage</title>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<h1>Basic HTML webpage</h1>\r\n"
		"<p>Hello, world!</p>\r\n"
		"</body>\r\n"
		"</html>\r\n";
	return (header + body);
}

int	setUpServer()
{
	struct sockaddr_in server_addr;
	int serverSocket;
	int ret;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8080);

	//serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // atomique, a la place d'utiliser fcntl avec O_NONBLOCK
	if (serverSocket < 0) {
		perror("ERROR! serverSocket: ");
		return (-1);
	}

	// Pouvoir retry sans erreur avant 60s (reutiliser 8080):
	int	enable = 1;
	if	(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
		perror("ERROR! setsockopt: ");
		return -1;
	}

	ret = bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (ret < 0)
		perror("ERROR! bind: ");

	ret = listen(serverSocket, SOMAXCONN);
	if (ret < 0)
		perror("ERROR! listen: ");

	return (serverSocket);
}

Connection	add_clientFD_to_epoll(int epollFd, int listenSocket)
{
	Connection	res;
	socklen_t	client_addr_len = sizeof(res.clientAddr);
	struct epoll_event	ev_hints;

	res.clientFd = accept(listenSocket, (struct sockaddr *) &res.clientAddr, &client_addr_len);
	if (res.clientFd < 0) {
		perror("ERROR! accept: ");
		throw std::runtime_error("accept socket failed");
	}

	fcntl(res.clientFd, F_SETFL, O_NONBLOCK);

	ev_hints.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
	ev_hints.data.fd = res.clientFd;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, res.clientFd, &ev_hints);
	return (res);
}

void	main_loop(int epollFd, int listenSocket)
{
	struct epoll_event			ready_events[MAX_EVENTS];
	std::map<int, Connection>	connections;

	while (1) {
		int efd_count = epoll_wait(epollFd, ready_events, MAX_EVENTS, 100);
		if (efd_count < 0)
			perror("ERROR! epoll_wait: ");

		for (int i = 0; i < efd_count; i++) {
			if (ready_events[i].data.fd == listenSocket) {
				Connection newConnection = add_clientFD_to_epoll(epollFd, listenSocket);
				connections[newConnection.clientFd] = newConnection;
			}
			else {
				std::map<int, Connection>::iterator	it;
				it = connections.find(ready_events[i].data.fd);
				if (it != connections.end()) {
					Connection& currConn = it->second;

					currConn.receiveRequest();
					currConn.parseRequest();
					currConn.sendResponse();
				}

				ssize_t	bytes_recvd;
				ssize_t	bytes_sent;
				ssize_t	total_sent;
				char	request_to_parse[10];
				std::string	tmp;
				std::string	request;
				std::string response;
				const char*	data;

				do {
					bytes_recvd = recv(ready_events[i].data.fd, (void *) request_to_parse, sizeof(request_to_parse), 0);
					if (bytes_recvd > 0) {
						std::string	tmp(request_to_parse, bytes_recvd);
						request.append(tmp);
					}
					std::cout << "bytes read: " << bytes_recvd << std::endl;

				} while (bytes_recvd > 0);

				if (bytes_recvd < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK))
					perror("ERROR! recv :");

				if (bytes_recvd == 0 && ready_events[i].events & EPOLLRDHUP) {
					std::cout << "SIGHUP flag was set\n";
					epoll_ctl(epollFd, EPOLL_CTL_DEL, ready_events[i].data.fd, NULL);
					close(ready_events[i].data.fd);
					continue ;
				}


				total_sent = 0;
				ssize_t length = response.length();
				response = build_response();
				data = response.c_str();
				while (total_sent < length) {
					bytes_sent = send(ready_events[i].data.fd, data + total_sent, response.length() - total_sent, MSG_NOSIGNAL);
					if (bytes_sent < 0) {
						perror("ERROR! send: ");
						break ;
					}
					total_sent += bytes_sent;
				}

			}
		}
	}
}

int	main()
{
	int	listenSocket;
	struct epoll_event	ev_hints;

	listenSocket = setUpServer();
	if (listenSocket < 0)
		return (1);

	int epollFd = epoll_create1(0);
	if (epollFd < 0) {
		perror("ERROR! epoll: ");
		return (1);
	}

	ev_hints.events = EPOLLIN;
	ev_hints.data.fd = listenSocket;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, listenSocket, &ev_hints);

	try {
		main_loop(epollFd, listenSocket);
	}
	catch (std::exception &e) {
		std::cerr << "Exception happened: " << e.what() << std::endl;
	}

	return (0);
}
