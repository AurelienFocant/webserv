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
#include "PathContext.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "resp.hpp"

class Connection;
class VirtualServer;

class RequestHandler
{
	private:

	/*PRIVATE ATTRIBUTES */
	const VirtualServer*	_server;
	PathContext				_ctx;


	public:

	/* PUBLIC METHODS */
	/* Constructors / Destructors */
	RequestHandler	();
	~RequestHandler	();

	RequestHandler&	operator=(const RequestHandler& rhs);

	/* Public Attributes */
	Request			_request;
	Response		_response;


	/* Main method*/
	void			processRequest(const std::string& inpput);
	void			processBody();
	void			handleRequest();
	void			findLocation();
	bool			isAllowedMethod();
	void			resetPathContext();
	void			requestIsComplete();
	void 			clean();

	/* Getters */
	const Request&			getRequest()	const ;
	const Response&			getResponse()	const ;
	const VirtualServer*	getVirtualServer()	const ;


	//std::string				getRoot()		const {return _root;}

	/* CGI Getters */
	std::string				getQuery()	const { return (_ctx.query);};
	std::string				getPathInfo()	const { return (_ctx.path_info);};
	std::string				getScriptName()	const { return (_ctx.script_name);};
	t_extension				getExtension()	const	{ return (_ctx.matched_extension);};
	std::string				getResolvedPath()	const	{ return (_ctx.resolved_path);};
	std::string				getCGIExec()		const	{return _ctx.cgi_exec;}

	/* Setters */
	void					setVirtualServer(const VirtualServer& server);
	void					setRoot(const std::string& root);
	//void					setContext(const )

};

#endif
