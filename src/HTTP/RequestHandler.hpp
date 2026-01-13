#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include "Request.hpp"
#include "Response.hpp"
//#include "../Webserv/VirtualServer.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

class Location 
{
	private:

	/* Private Attributes */
	std::string					_name;
	std::string					_root;
	std::string					_alias;
	std::string					_redirect;
	int							_redirect_code;
	std::vector<std::string>	_index;
	bool						_autoindex;

	public:

	/* Constructors / Destructors */
	Location	();
	Location	(const Location& other);
	Location&	operator= (const Location& rhs);
	~Location	();

	/* Getters */
	std::string					getName() const {return _name;}
	std::string					getRoot() const {return _root;}
	std::string					getAlias() const {return _alias;}
	std::string					getRedirect() const {return _redirect;}
	int							getRedirectCode() const {return _redirect_code;}
	std::vector<std::string>	getIndex() const {return _index;}
	bool						getAutoIndex() const {return _autoindex;}


	/* Setters */
	void	setName(const std::string& name) {_name = name;}
	void	setRoot(const std::string& root) {_root = root;}
	void	setAlias(const std::string& alias) {_alias = alias;}
	void	setRedirect(const std::string& redirect) {_redirect = redirect;}
	void	setRedirectCode(const int& redirect_code) {_redirect_code = redirect_code;}
	void	setIndex(const std::vector<std::string>& index) {_index = index;}
	void	setAutoIndex(bool autoindex) {_autoindex = autoindex;}

	void	addIndexFile(const std::string& file) {_index.push_back(file);}
};

class RequestHandler
{
	private:

	/* Private Attributes */
	const Request&			_request;
	Response&				_response;
	//const VirtualServer&	_virtual_server;

	std::string			_root; // default root from config
	std::string			_request_path; // request_uri sans query
	std::string			_full_path; //defnitive path (after alias or override)

	const Location*		_matched_location;
	bool				_is_directory;

	int					_status_code;
	bool				_has_error;

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
	//RequestHandler	();
	RequestHandler	(const Request& request, Response& response);
	~RequestHandler	();

	/* Getters */
	std::string		getRoot() const {return _root;}
	int				getStatusCode() const {return _status_code;}
	bool			hasError() const {return _has_error;}

	/* Public Methods */
	void			handleRequest();
	std::string		buildResponse();
};

#endif
