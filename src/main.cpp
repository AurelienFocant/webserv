#include <stdlib.h>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>
#include <string.h>
#include <poll.h>
#include <sys/epoll.h>

#include <vector>

#include <fcntl.h>
#include <unistd.h>

#define MAX_EVENTS 1024

std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 637\r\nCache-Control: s-maxage=300, public, max-age=0\r\nContent-Language: en-US\r\nDate: Thu, 06 Dec 2018 17:37:18 GMT\r\nETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"\r\nServer: meinheld/0.6.1\r\nStrict-Transport-Security: max-age=63072000\r\nX-Content-Type-Options: nosniff\r\nX-Frame-Options: DENY\r\nX-XSS-Protection: 1; mode=block\r\nVary: Accept-Encoding,Cookie\r\nAge: 7\r\n\r\n<!doctype html>\r\n<html lang=\"en\">\r\n<head>\r\n<meta charset=\"utf-8\">\r\n<title>A basic webpage</title>\r\n</head>\r\n<body>\r\n<h1>Basic HTML webpage</h1>\r\n<p>Hello, world!</p>\r\n</body>\r\n</html>\r\n";

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
	if (serverSocket < 0)
	{
			perror("serverSocket: ");
			return (-1);
	}
	
	// Pouvoir retry sans erreur avant 60s (reutiliser 8080):
	int	enable = 1;
	if	(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
	{
		perror("setsockopt: ");
		return -1;
	}

	ret = bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (ret < 0)
		perror("bind: ");

	ret = listen(serverSocket, SOMAXCONN);
	if (ret < 0)
		perror("listen: ");

	return (serverSocket);
}


int	main()
{
	int listenSocket;
	struct epoll_event ev, ready_events[MAX_EVENTS];
	
	listenSocket = setUpServer();
	if (listenSocket < 0)
		return (1);

	//fcntl(listenSocket, F_SETFL, O_NONBLOCK);

	int e_fd = epoll_create1(0);
	std::cout << "epoll fd: " << e_fd << std::endl;
	if (e_fd < 0)
	{
		perror("epoll: ");
		return (1);
	}

	ev.events = EPOLLIN;
	ev.data.fd = listenSocket;
	epoll_ctl(e_fd, EPOLL_CTL_ADD, listenSocket, &ev);

	while (1) {
		int	clientFd = 0;
		struct sockaddr_in	client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		char ip_buf[INET_ADDRSTRLEN];

		int efd_count = epoll_wait(e_fd, ready_events, MAX_EVENTS, 100);
		if (efd_count < 0)
			perror("epoll_wait: ");

		for (int i = 0; i < efd_count; i++)
		{
			std::cout << "READY FD: " << ready_events[i].data.fd << std::endl;
			 
			if (ready_events[i].data.fd == listenSocket)
			{
				clientFd = accept(listenSocket, (struct sockaddr *) &client_addr, &client_addr_len);
				if (clientFd < 0)
					perror("accept: ");

				fcntl(clientFd, F_SETFL, O_NONBLOCK);
				
				ev.events = EPOLLIN;
				ev.data.fd = clientFd;
				epoll_ctl(e_fd, EPOLL_CTL_ADD, clientFd, &ev);

				/* PRINT LOG CLIENT */
				std::cout << "client fd : " << clientFd << std::endl;
				inet_ntop(AF_INET, &client_addr.sin_addr, ip_buf, sizeof(ip_buf));
				std::cout << "connection from " << ip_buf << " client port: " << ntohs(client_addr.sin_port) << std::endl;
			}
			else
			{

				ssize_t	ret;
				char request_to_parse[1024];

				if (ready_events[i].events & EPOLLHUP) {
					std::cout << "SIGHUP flag was set\n";
				}

				std::string	request;
				do {
					ret = recv(ready_events[i].data.fd, (void *) request_to_parse, sizeof(request_to_parse) - 1, 0);
					if (ret > 0) {
						request_to_parse[ret] = '\0';
						request.append(request_to_parse);
					}
				} while (ret > 0);

				if (ret < 0)
					perror("recv :");

				if ((send(ready_events[i].data.fd, (const void *) response.c_str(), response.length(), MSG_NOSIGNAL)) < 0)
					perror("send: ");

				if (ret == 0)
					close(ready_events[i].data.fd);
				// -> parser la requete http blablabla
				
				std::cout << "PROBLEMOS?" << std::endl;

				// if erno -1 sur EAGAIN / EWOULDBLOCK continue NOPE :'(
				// ! send() peut renvoyer - que response.lenght / envoi partiel
			}
		}

/* 		std::cout << "Connection closed" << std::endl; */
	}
	return (0);
}
