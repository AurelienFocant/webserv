#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctime>

#include "HTTPenum.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "pathResolver.hpp"
#include "resp.hpp"

class Connection;
class VirtualServer;

class RequestHandler
{
	private:

	/*PRIVATE ATTRIBUTES */
	Request					_request;
	Response				_response;
	const VirtualServer*	_server;
	PathContext				_ctx;


	public:

	/* PUBLIC METHODS */
	/* Constructors / Destructors */
	RequestHandler	();
	~RequestHandler	();

	RequestHandler&	operator=(const RequestHandler& rhs);

	/* Public methods*/
	void					processRequest(std::string& input);
	void					processBody();
	void					handleRequest();
	void					findLocation();
	bool					isAllowedMethod();
	void					setRequestToComplete();
	bool					validCgiRequest();
	void					resetPathContext();
	void 					clean();

	/* Getters */
	const Request&			getRequest() const;
	const Response&			getResponse() const;
	Response&				getResponse();
	const VirtualServer*	getVirtualServer() const;

	/* CGI Getters */
	std::string				getQuery() const;
	std::string				getPathInfo() const;
	std::string				getScriptName() const;
	t_extension				getExtension() const;
	std::string				getResolvedPath() const;
	std::string				getCGIExec() const;

	/* Setters */
	void					setVirtualServer(const VirtualServer& server);
	void					setRoot(const std::string& root);

};

#endif
