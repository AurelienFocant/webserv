#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <ctime>

#include "Location.hpp"

class ConfigContext;

class VirtualServer
{
	private:
		unsigned int	_port;
		std::string		_root;
	 	std::string		_server_name;
		std::string		_redirect;
		int				_redirect_code;
		bool			_autoindex;
		std::time_t		_keepalive_time;
		std::time_t		_keepalive_timeout;
		int				_cgi_timeout;
		std::string		_cgi_exec;
		size_t			_max_body_size;

		std::map<int, std::string>		_error_pages;
		std::vector<std::string>		_indexes;
		std::set<std::string>			_allowed_methods;

		std::map<std::string, Location>	_locations;


	public:
		void	setPort(unsigned int port)									{_port = port;}
		void	setRoot(std::string root)									{_root = root;}
		void	setServName(std::string name)								{_server_name = name;}
		void	setAutoindex(bool b)										{_autoindex = b;}
		void	setIndexes(std::vector<std::string> const& src)				{_indexes.assign(src.begin(), src.end());}
		void	setErrorPages(std::map<int, std::string> ep)				{_error_pages = ep;}
		void	setErrorPage(int code, std::string page)					{_error_pages.insert(std::make_pair(code, page));}
		void	setLocations(const std::map<std::string, Location> &locs)	{_locations = locs;}
		void	setLocationsAt(std::string key, Location &loc)				{_locations.insert(std::make_pair(key, loc));}
		void	setMaxBodySize(long l)										{_max_body_size = l;}
		void	setCGITimeout(int n)										{_cgi_timeout = n;}
		void	setCGIExec(std::string x)									{_cgi_exec = x;}

		unsigned int	getPort(void)								const	{return _port;}
		std::string		getRoot(void)								const	{return _root;}
		std::string		getServName(void)							const	{return _server_name;}
		bool			getAutoindex(void)							const	{return _autoindex;}
		std::string		getErrorPage(int code)						const	{return _error_pages.at(code);}
		std::time_t		getKeepaliveTime(void)						const	{return _keepalive_time;}
		std::time_t		getKeepaliveTimeout(void)					const	{return _keepalive_timeout;}
		int				getCGITimeout()								const	{return _cgi_timeout;}	
		std::string		getCGIExec()								const	{return _cgi_exec;}
		size_t			getMaxBodySize()							const	{return _max_body_size;}
		std::string		getRedirect()								const	{return _redirect;}
		int				getRedirectCode()							const	{return _redirect_code;}
		std::time_t		getKeepalive_time()							const	{return _keepalive_time;}
		std::time_t		getKeepalive_timeout()						const	{return _keepalive_timeout;}
																	
		std::set<std::string>			const& getAllowedMethods()				const	{return _allowed_methods;}
		std::map<int, std::string>		const& getErrorPages()					const	{return _error_pages;}
		std::vector<std::string>		const& getIndexes(void)					const	{return _indexes;}
		std::map<std::string, Location>	const& getLocations(void)				const	{return _locations;}
		Location						const& getLocationAt(std::string key)	const	{return _locations.at(key);}

		VirtualServer	( void );
		VirtualServer	(ConfigContext const& ctxt);
		VirtualServer	( const VirtualServer& src );
		VirtualServer&	operator= ( const VirtualServer& rhs );
		~VirtualServer	( void );
};

#endif // VIRTUALSERVER_HPP
