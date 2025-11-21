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

class Connection
{
	public:
		int					clientFd;
		struct sockaddr_in	clientAddr;
		std::string			request;
		std::string			response;
		int					respOffset;
		bool				connClosed;
		struct epoll_event	epollEvent;

		bool	receiveRequest() {
			char	buf[10];
			int		bytesRecvd;


			while ((bytesRecvd = recv(clientFd, buf, sizeof(buf), 0)) > 0) {
				request.append(buf, bytesRecvd);
			}

			if (bytesRecvd == 0 && epollEvent.events & EPOLLRDHUP) {
				connClosed = true;
				return false;
			}
			if (epollEvent.events & EPOLLERR || epollEvent.events & EPOLLHUP) {
				connClosed = true;
				return false;
			}

			connClosed = false;
			return (true);
		};

		bool	parseRequest() {
			return (true);
		};

		void	sendResponse(int epollFd) {
			ssize_t bytesSent = send(clientFd,
									response.c_str() + respOffset,
									response.length() - respOffset,
									MSG_NOSIGNAL);

			if (bytesSent > 0) {
				respOffset += bytesSent;

				if (respOffset >= (int)response.length()) {
					// full response sent --> stop watching EPOLLOUT
					struct epoll_event	ev;

					ev.events = EPOLLIN | EPOLLRDHUP; // keep listening for reads
					ev.data.fd = clientFd;
					if (epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev) < 0) {
						perror("epoll_ctl MOD");
						connClosed = true;
					}

					response.clear();
					respOffset = 0;
				}
			}
			else if (bytesSent < 0) {
				if (errno != EAGAIN && errno != EWOULDBLOCK) {
					perror("send");
					connClosed = true;
				}
			}
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
	struct epoll_event	ev;

	res.clientFd = accept(listenSocket, (struct sockaddr *) &res.clientAddr, &client_addr_len);
	if (res.clientFd < 0) {
		perror("ERROR! accept: ");
		throw std::runtime_error("accept socket failed");
	}

	fcntl(res.clientFd, F_SETFL, O_NONBLOCK);

	ev.events = EPOLLIN | EPOLLRDHUP;
	ev.data.fd = res.clientFd;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, res.clientFd, &ev);
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
			int	fd = ready_events[i].data.fd;

			if (fd == listenSocket) {
				Connection newConnection = add_clientFD_to_epoll(epollFd, listenSocket);
				newConnection.epollEvent = ready_events[i];
				connections[newConnection.clientFd] = newConnection;
			}
			else {
				std::map<int, Connection>::iterator	it;
				it = connections.find(fd);
				if (it != connections.end()) {
					Connection& currConn = it->second;

					if (ready_events[i].events & EPOLLIN) {
						currConn.receiveRequest();
						if (currConn.connClosed) {
							epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
							close(fd);
							connections.erase(it);
							continue ;
						}
					}

					currConn.parseRequest();

					if (currConn.response.empty()) {
						struct epoll_event	ev;
						ev.events = EPOLLOUT | EPOLLRDHUP;
						ev.data.fd = currConn.clientFd;
						epoll_ctl(epollFd, EPOLL_CTL_MOD, currConn.clientFd, &ev);
						currConn.response = build_response();
					}

					if (ready_events[i].events & EPOLLOUT) {
						currConn.sendResponse(epollFd);
						if (currConn.connClosed) {
							epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
							close(fd);
							connections.erase(it);
							continue ;
						}
					}

					if (ready_events[i].events & EPOLLERR || ready_events[i].events & EPOLLHUP) {
						epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
						close(fd);
						connections.erase(it);
					}
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
