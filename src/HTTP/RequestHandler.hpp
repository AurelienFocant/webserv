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

	/* Path Resolution */
	std::string				_root;
	std::string				_request_path;
	std::string				_resolved_path;
	std::string				_cgi_exec;
	const Location*			_matched_location;
	t_extension				_matched_extension;
	bool					_is_directory;

	/* CGI ENV */
	std::string				_script_name;
	std::string				_path_info;
	std::string				_query;

	/* PRIVATE METHODS */
	void			addInput(const std::string& input);

	/* Path processing */
	bool			extractPath();
	bool			resolvePath();
	bool			validatePath();

	bool			decodePath(const std::string& encoded, std::string& decoded);
	bool			normalizePath();
	bool			detectCGI();

	/*  Redirections */
	bool			handleConfigRedirect();
	bool			handleTrailingSlash();
	bool			hasRedirect();

	/* Request processing */
	bool			processMethods();
	bool			processGetMethod();
	bool			processHeadMethod();
	bool			processPostMethod();
	bool			processDeleteMethod();
	bool			_hasContentTypeHeader();
	bool			_isMultiformData();
	std::string		_extractBoundary();
	std::string		_extractFilename(std::string boundary);
	std::string		_verifyFile(std::string filename);
	bool			_saveDataToFile(std::string filename);

	/* Directory listing / Index */
	bool			resolveIndex();
	bool			hasAutoIndex();
	void			generateAutoIndex();

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
	void			findLocation();
	bool			isAllowedMethod();

	void			clean();

	/* Getters */
	std::string				getRoot()		const {return _root;}
	const Request&			getRequest()	const ;
	const Response&			getResponse()	const ;
	const VirtualServer*	getVirtualServer()	const ;
	std::string				getQuery()	const { return (_query);};
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
