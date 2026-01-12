#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "Request.hpp"
#include "Response.hpp"
#include "VirtualServer.hpp"
#include "Location.hpp"

class RequestHandler
{
	private:

	/* Private Attributes */
	const Request&		_request;
	Response&			_response;
	//const ServerConfig& _config; -> findServer() = identifier le bon serveur grace a son port/ip et hostname si besoin
	std::string			_root; // default root from config
	std::string			_path; // request_uri sans query
	std::string			_fullPath; //defnitive path (after alias or override)
	//std::string			_index; //define file(s)to search when URI point to a dir
	const Location*			_matchedLocation;
	bool				_isDirectory;
	int					_statusCode;
	bool				_hasError;

	std::map<std::string, Location> _routes; //devrait etre dans config, juste pour tests

	/* Private Methods */
	bool			extractPath();
	bool			resolvePath();
	bool			validatePath();
	void			findLocation();
	bool			processMethods();
	void			processGetMethod();
	std::string 	getContentType(const std::string& path);
	int				openReadFile(const std::string& path);
	int				openWriteFile(const std::string& path);
	bool			resolveIndex();
	bool 			isDirectory(const std::string& path);
	size_t			fileSize(const std::string& path);

	/* For testing */
	void 			initRoutes();
	void			printRoutes();

	public:

	/* Constructors / Destructors */
	//RequestHandler	();
	RequestHandler	(const Request& request, Response& response);
	~RequestHandler	();

	/* Getters */
	std::string		getRoot() const {return _root;}
	int				getStatusCode() const {return _statusCode;}
	bool			hasError() const {return _hasError;}

	/* Public Methods */
	void			handleRequest();
	std::string		buildResponse();
};

#endif
