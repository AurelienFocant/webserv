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
#include "resp.hpp"
#include "../Utils/fileSystem.hpp"
#include "../Utils/httpUtils.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Connection;
class Response;
class Request;
class Location;
class VirtualServer;

class RequestHandler
{
	private:

	/*PRIVATE ATTRIBUTES */
	/* Objects */
	const VirtualServer*	_server;


	/* PRIVATE METHODS */
	void			addInput(const std::string& input);

	/* Request processing */
;
	bool			_hasContentTypeHeader();
	bool			_isMultiformData();
	std::string		_extractBoundary();
	std::string		_extractFilename(std::string boundary);
	std::string		_verifyFile(std::string filename);
	bool			_saveDataToFile(std::string filename);

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
	void			handleRequest();
	void			processRequest(const std::string& request_str);
	void			processBody();
	

	void			clean();

	/* Getters */
	std::string				getRoot()		const {return _root;}
	const Request&			getRequest()	const ;
	const Response&			getResponse()	const ;
	const VirtualServer*	getVirtualServer()	const ;
	std::string				getQuery()	const { return (_query);};
	std::string				getPathInfo()	const { return (_path_info);};
	std::string				getScriptName()	const { return (_script_name);};
	t_extension				getExtension()	const	{ return (_matched_extension);};
	std::string				getResolvedPath()	const	{ return (_resolved_path);};
	std::string				getCGIExec()		const	{return _cgi_exec;}

	/* Setters */
	void					setVirtualServer(const VirtualServer& server);
	void					setRoot(const std::string& root);
	void					requestIsComplete();

};

#endif
