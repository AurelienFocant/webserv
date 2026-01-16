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
	bool			extractPath();
	bool			resolvePath();
	bool			validatePath();
	void			findLocation();

	bool			processMethods();
	void			processGetMethod();

	bool			resolveIndex();
	bool			hasAutoIndex();
	void			generateAutoIndex();

	int				openReadFile(const std::string& path);
	int				openWriteFile(const std::string& path);
	bool 			isDirectory(const std::string& path);
	size_t			fileSize(const std::string& path);
	std::string 	getContentType(const std::string& path);

	/* For testing */
	void			printRoutes();

	public:

	/* Constructors / Destructors */
	RequestHandler	(Connection* currConn); // en const ref ou pointeur?
	~RequestHandler	();

	/* Getters */
	std::string		getRoot() const {return _root;}

	/* Public Methods */
	void			handleRequest();
};

std::string			intToString(size_t value);


#endif
