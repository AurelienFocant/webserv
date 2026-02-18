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


#include "ResponseBuilder.hpp"
#include "HTTPenum.hpp"
#include "../Utils/fileSystem.hpp"
#include "../Utils/httpUtils.hpp"

class Connection;
class Response;
class Request;
class Location;
class VirtualServer;
class ResponseBuilder;

class RequestHandler
{
	private:

	/*PRIVATE ATTRIBUTES */
	/* Objects */
	const Request&			_request;
	Response&				_response;
	VirtualServer&			_server;
	ResponseBuilder			_builder;

	/* Path Resolution */
	std::string				_root;
	std::string				_cage_root;
	std::string				_request_path;
	std::string				_resolved_path;
	const Location*			_matched_location;
	t_extension				_matched_extension;
	bool					_is_directory;

	/* CGI ENV */
	std::string				_script_name;
	std::string				_path_info;
	std::string				_query;

	/* PRIVATE METHODS */
	/* Path processing */
	bool			extractPath();
	bool			resolvePath();
	bool			validatePath();
	void			findLocation();

	bool			decodePath(const std::string& encoded, std::string& decoded);
	bool			normalizePath();
	bool			detectCGI();

	/*  Redirections */
	bool			handleConfigRedirect();
	bool			handleTrailingSlash();
	bool			hasRedirect();

	/* Request processing */
	bool			isAllowedMethod();
	bool			processMethods();
	bool			processGetMethod();
	bool			processPostMethod();
	bool			processDeleteMethod();
	bool			_hasContentTypeHeader();
	bool			_isMultiformData();
	std::string		_extractBoundary();

	/* GET Method */

	/*POST Method*/
	bool			createNewUser() ;

	/* Directory listing / Index */
	bool			resolveIndex();
	bool			hasAutoIndex();
	void			generateAutoIndex();

	/* For testing */
	void			printRoutes();

	public:

	/* PUBLIC METHODS */
	/* Constructors / Destructors */
	RequestHandler	(Connection& currConn);
	~RequestHandler	();

	RequestHandler&	operator=(const RequestHandler& rhs);

	/* Main method*/
	void			handleRequest();

	/* Getters */
	std::string		getRoot() const {return _root;}
	const Request&	getRequest() const ;
	const Response&	getResponse() const ;
	std::string		getQuery() const { return (_query);};
	std::string		getScriptName() const { return (_script_name);};
	t_extension		getExtension() const { return (_matched_extension);};
	std::string		getResolvedPath() const { return (_resolved_path);};

};

#endif
