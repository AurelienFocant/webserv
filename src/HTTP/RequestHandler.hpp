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
	VirtualServer			_server;

	std::string				_root; // default root from _virtual_server -> a supprimer
	std::string				_request_path; // request_uri sans query
	std::string				_resolved_path; //defnitive path (after alias or override)

	const Location*			_matched_location;
	bool					_is_directory;

	/* Private Methods */

	/* Path processing */
	bool			extractPath();
	bool			resolvePath();
	bool			validatePath();
	void			findLocation();

	/*  Redirections */
	bool			handleConfigRedirect();
	bool			handleTrailingSlash();
	bool			hasRedirect();

	/* Request processing */
	bool			processMethods();
	void			processGetMethod();
	bool			processPostMethod();
	/*POST Methods*/
	//void	processPostMethod() ;
	bool			createNewUser() ;

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
	RequestHandler	(Connection& currConn); // en const ref ou pointeur?
	~RequestHandler	();

	/* Getters */
	std::string		getRoot() const {return _root;}

	/* Public Methods */
	void			handleRequest();
};

std::string			intToString(size_t value);


#endif
