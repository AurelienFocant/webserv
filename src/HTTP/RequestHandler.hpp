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

#include "Connection.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "VirtualServer.hpp"
#include "Location.hpp"
#include "../Utils/fileSystem.hpp"
#include "../Utils/httpUtils.hpp"

//class ResponseBuilder;

class RequestHandler
{
	private:

	/* Private Attributes */
	const Request&			_request;
	Response&				_response;
	VirtualServer&			_server;
	ResponseBuilder			_builder;

	std::string				_root;
	std::string				_cage_root;
	std::string				_request_path;
	std::string				_resolved_path;

	std::string				_script_name;
	std::string				_path_info;
	std::string				_query;

	const Location*			_matched_location;
	t_extension				_matched_extension;
	bool					_is_directory;

	/* Private Methods */

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

	/* GET Method */
	bool			executeCGI(); // a implementer

	/*POST Method*/
	//void	processPostMethod() ;
	bool			createNewUser() ;

	/* Directory listing / Index */
	bool			resolveIndex();
	bool			hasAutoIndex();
	void			generateAutoIndex();

	/* For testing */
	void			printRoutes();

	public:

	/* Constructors / Destructors */
	RequestHandler	(Connection& currConn);
	~RequestHandler	();

	RequestHandler&	operator=(const RequestHandler& rhs);

	/* Getters */
	std::string		getRoot() const {return _root;}
	const Request&	getRequest() const ;
	const Response&	getResponse() const ;

	/* Public Methods */
	void			handleRequest();
};

#endif
