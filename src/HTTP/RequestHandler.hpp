#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "Connection.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "VirtualServer.hpp"
#include "Location.hpp"

class RequestHandler
{
	private:

	/* Private Attributes */
	const Request&			_request;
	Response&				_response;
	VirtualServer&	_server;

	std::string				_root;
	std::string				_request_path; // request_uri sans query
	std::string				_resolved_path; //defnitive internal path (after alias or override)

	std::string				_query;

	const Location*			_matched_location;
	t_extension				_matched_extension;
	bool					_is_directory;

	/* Private Methods */

	/* Path processing */
	bool			extractPath();
	bool			resolvePath();
	bool			normalizePath();
	bool			validatePath();
	void			findLocation();

	bool			detectCgi();


	/*  Redirections */
	bool			handleConfigRedirect();
	bool			handleTrailingSlash();
	bool			hasRedirect();

	/* Request processing */
	bool			processMethods();
	bool			processGetMethod();

	/* Directory listing / Index */
	bool			resolveIndex();
	bool			hasAutoIndex();
	void			generateAutoIndex();

	/* Build Response */
	void			buildFileResponse(int fd);
	void			buildHtmlResponse(const std::string& content);
	void			buildRedirectResponse(int status_code, const std::string& redirect_uri);
	void			buildErrorResponse(int status_code);

	/* Errors */
	bool			loadErrorPage(int status_code, int& fd, size_t& size);
	std::string		generateDefaultError(int status_code);

	/* File operations */
	int				openReadFile(const std::string& path);
	int				openWriteFile(const std::string& path);
	bool 			isDirectory(const std::string& path);
	size_t			fileSize(const std::string& path);
	std::string 	getContentType(const std::string& path);

	/* For testing */
	void			printRoutes();

	public:

	/* Constructors / Destructors */
	RequestHandler	(Connection& currConn);
	~RequestHandler	();

	/* Getters */
	std::string		getRoot() const {return _root;}

	/* Public Methods */
	void			handleRequest();
};

std::string			intToString(size_t value);


#endif
