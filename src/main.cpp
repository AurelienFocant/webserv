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


// void	main_loop(Webserv & webserv, int epollFd, int listenSocket)
// {
// 	struct epoll_event			ready_events[MAX_EVENTS];
// 	std::map<int, Connection*>	connections;
//
// 	while (1) {
//
// 					if (ready_events[i].events & EPOLLIN) {
// 						currConn->receiveRequest();
//
// 						// Close Connection if needed
// 						if (currConn->connClosed) {
// 							epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
// 							close(fd);
// 							connections.erase(it);
// 							continue ;
// 						}
// 						// We'll need to do loads of stuff in here
// 						std::cout << "main_loop -l129: "<< currConn->request << std::endl;
// 					}
//
//
// 					// is empty --> build it
// 					// and tell epoll we want it to tell us
// 					// when the socket is ready for writing
// 					if (currConn->request.getCompleted() && currConn->response_str.empty()) {
//
// 						currConn->virtual_server = webserv.getValidServer(0);
// 						/* TEST REQUEST HANDLER */
// 						std::cout << "Main 147: Request Handler" << std::endl;
// 						std::cout << "Status Code: " << currConn->request.getStatusCode() << std::endl;
// 						RequestHandler rHandler(currConn->request, currConn->response);
// 						rHandler.handleRequest();
// 						if (rHandler.hasError())
// 						{
// 							std::cerr << "Error: " << rHandler.getStatusCode() << std::endl;
// 							// build error response
// 						}
// /* 						else 
// 							currConn->response = rHandler.buildResponse(); */
// 						/* --------------------- */
//
// 						struct epoll_event	ev;
// 						ev.events = EPOLLOUT | EPOLLRDHUP;
// 						ev.data.fd = currConn->clientFd;
// 						epoll_ctl(epollFd, EPOLL_CTL_MOD, currConn->clientFd, &ev);
// 						//currConn->response = currConn->build_response();
// 						currConn->request.cleanRequest();
// 						currConn->sendResponse(epollFd);
// 					}
//
// 					int ret2 = ready_events[i].events & EPOLLOUT;
// 					std::cout << "RET EPOLLOUT: " << ret2 << std::endl;
//
// 					// If the socket is ready for writing
// 					if (ready_events[i].events & EPOLLOUT) {
// 						currConn->sendResponse(epollFd);
//
// 						// Close Connection if needed
// 						if (currConn->connClosed) {
// 							epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
// 							close(fd);
// 							connections.erase(it);
// 							continue ;
// 						}
// 					}
//
// 					// Close connection if error
// 					if (ready_events[i].events & EPOLLERR || ready_events[i].events & EPOLLHUP) {
// 						epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
// 						close(fd);
// 						connections.erase(it);
// 					}
// 				}
// 			}
// 		}
// 	}
// }

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
		webserv.extractServerRoot(av[0]);
		webserv.run();
	}
	catch (std::exception &e) {
		std::cerr << "Exception happened: " << e.what() << std::endl;
		return (2);
	}

	return (0);
}
