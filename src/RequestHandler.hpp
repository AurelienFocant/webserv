#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include <errno.h>
#include "Request.hpp"

#include <unistd.h>
#include <fcntl.h>
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
	std::string	getAliast() const {return _alias;}

	/* Setters */
	void	setName(const std::string& name) {_name = name;}
	void	setRoot(const std::string& root) {_root = root;}
	void	setAlias(const std::string& alias) {_alias = alias;}
};


/* class ServerConfig 
{
	public: 

	ServerConfig	();
	ServerConfig	(const ServerConfig& other);
	ServerConfig&	operator= (const ServerConfig& rhs );
	~ServerConfig	();

	std::string _root;
	std::map<std::string, Location> route;
	std::map<int, std::string> errors_pages;

	//std::string	getRoot() const {return _root;}
	//std::string	getLocation() const {return _root;}
};

ServerConfig::ServerConfig () {}
ServerConfig::~ServerConfig () {} */


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
	int					_statusCode;

	/* Private Methods */
	void	extractPath();
	void	resolvePath();
	bool	validatePath();

	public:

	/* Constructors / Destructors */
	//RequestHandler	();
	RequestHandler	(const Request& request);
	~RequestHandler	();

	/* Getters */
	std::string	getRoot() const {return _root;}

	/* Public Methods */
	void		handleRequest();
	std::string	buildResponse();
};

#endif