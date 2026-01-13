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
	const Request&			_request;
	Response&			_response;
	//const VirtualServer&	_virtual_server;

	std::string			_root; // default root from _virtual_server -> a supprimer
	std::string			_request_path; // request_uri sans query
	std::string			_resolved_path; //defnitive path (after alias or override)

	const Location*			_matched_location;
	bool				_is_directory;

	int				_status_code; // -> plutot dans Response
	bool				_has_error; // redondant

	std::map<std::string, Location> _routes; //devrait etre dans config, juste pour tests

	/* Private Methods */
	bool			extractPath();
	bool			resolvePath();
	bool			validatePath();
	void			findLocation();

	bool			processMethods();
	void			processGetMethod();

	bool			resolveIndex();
	bool			hasAutoIndex();
	void			generateAutoIndex();
	//void			generateAutoIndex();

	int				openReadFile(const std::string& path);
	int				openWriteFile(const std::string& path);
	bool 			isDirectory(const std::string& path);
	size_t			fileSize(const std::string& path);
	std::string 	getContentType(const std::string& path);

	/* For testing */
	void 			initRoutes();
	void			printRoutes();

	public:

	/* Constructors / Destructors */
	RequestHandler	(const Request& request, Response& response);
	~RequestHandler	();

	/* Getters */
	std::string		getRoot() const {return _root;}
	int				getStatusCode() const {return _status_code;}
	bool			hasError() const {return _has_error;}

	/* Public Methods */
	void			handleRequest();
};

#endif
