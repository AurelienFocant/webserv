#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include "Request.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

class Location 
{
	private:

	/* Private Attributes */
	std::string _name;
	std::string _root;
	std::string _alias;

	public:

	/* Constructors / Destructors */
	Location	();
	Location	(const Location& other);
	Location&	operator= (const Location& rhs);
	~Location	();

	/* Getters */
	std::string	getName() const {return _name;}
	std::string	getRoot() const {return _root;}
	std::string	getAlias() const {return _alias;}

	/* Setters */
	void	setName(const std::string& name) {_name = name;}
	void	setRoot(const std::string& root) {_root = root;}
	void	setAlias(const std::string& alias) {_alias = alias;}
};

class RequestHandler
{
	private:

	/* Private Attributes */
	const Request&		_request;
	//const ServerConfig& _config;
	std::string			_root; // default root from config
	std::string			_path; // request_uri sans query
	std::string			_fullPath; //defnitive path (after alias or override)
	//std::string			_index; //define file(s)to search when URI point to a dir
	bool				_isDirectory;
	int					_statusCode;
	bool				_hasError;

	std::map<std::string, Location> _routes; //devrait etre dans config, juste pour tests

	/* Private Methods */
	bool		extractPath();
	bool		resolvePath();
	bool		validatePath();
	bool		processMethods();
	void		processGetMethod();
	int			openReadFile(const std::string& path);
	int			openWriteFile(const std::string& path);
	bool		resolveIndex();
	bool 		isDirectory(const std::string& path);
	size_t		fileSize(const std::string& path);

	/* For testing */
	void 		initRoutes();
	void		printRoutes();

	public:

	/* Constructors / Destructors */
	//RequestHandler	();
	RequestHandler	(const Request& request);
	~RequestHandler	();

	/* Getters */
	std::string		getRoot() const {return _root;}
	int				getStatusCode() const {return _statusCode;}
	bool			hasError() const {return _hasError;}

	/* Public Methods */
	void			handleRequest();
	std::string		buildResponse();
};

/* class ServerConfig 
{
	public: 

	ServerConfig	();
	ServerConfig	(const ServerConfig& other);
	ServerConfig&	operator= (const ServerConfig& rhs );
	~ServerConfig	();

	std::string _root;
	std::map<std::string, Location> routes;
	std::map<int, std::string> errors_pages;

	//std::string	getRoot() const {return _root;}
	//std::string	getLocation() const {return _root;}
};

ServerConfig::ServerConfig () {}
ServerConfig::~ServerConfig () {} */

#endif