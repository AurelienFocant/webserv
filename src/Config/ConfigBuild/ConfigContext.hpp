#pragma once
#include <string>
#include <map>
#include "VirtualServer.hpp"
#include <ctime>
#include <set>

enum ContextType {
    MAIN = 1,
    SERVER,
    LOCATION
};

class ConfigContext {
	private:
		ContextType _type;

		int         _port;
		std::string _root;
		std::string	_alias;
		std::string _serverName;
		std::string _locationName;
		bool		_autoindex;
		std::time_t	_keepalive_time;
		std::time_t	_keepalive_timeout;
		int			_redirect_code;
		std::string	_redirect;
		bool		_cgi;
		bool		_virtualLocation;

		std::map<int, std::string>		_error_pages;
		std::vector<std::string>		_indexes;
		std::set<std::string>			_allowed_methods;
		std::map<std::string, Location> _locations;

		std::map<int, std::string>	_initDefaultErrorPages(void);

	public:
		void	inheritFrom(const ConfigContext &parent);


									ContextType getType()				const	{return _type;}
									int         getPort()				const	{return _port;}
									std::string getRoot()				const	{return _root;}
									std::string getServerName()			const	{return _serverName;}
									std::string getLocationName()		const	{return _locationName;}
		 std::map<std::string, Location> const& getLocations()			const	{return _locations;}
									bool		getAutoindex() 			const	{return _autoindex;}
				std::vector<std::string> const&	getIndexes()   			const	{return _indexes;}
									int			getRedirectCode()		const	{return _redirect_code;}
									std::string	getRedirect()			const	{return _redirect;}
									std::time_t	getKeepalive_time()		const	{return _keepalive_time;}
									std::time_t	getKeepalive_timeout()	const	{return _keepalive_timeout;}
						std::set<std::string>	getAllowedMethods()		const	{return _allowed_methods;}
									bool		getCGI()	   			const	{return _cgi;}
									bool		getVirtualLocation()	const	{return _virtualLocation;}
									std::string	getAlias()				const	{return _alias;}
					std::map<int, std::string>	getErrorPages()	   		const	{return _error_pages;}
									std::string	getErrorPage(int code)  const	{return _error_pages.at(code);}

		void        setPort(int port)											{_port = port;}
		void        setRoot(const std::string &root)							{_root = root;}
		void        setServerName(const std::string &serverName)				{_serverName = serverName;}
		void		setLocationName(const std::string &locationName)			{_locationName = locationName;}
		void		addLocation(const std::string &name, const Location &loc)	{_locations.insert(std::make_pair(name, loc));}
		void		setIndexes(std::vector<std::string> const& src)				{_indexes.assign(src.begin(), src.end());}
		void		setAutoindex(bool b)										{_autoindex = b;}
		void		setRedirectCode(int code)									{_redirect_code = code;}
		void		setRedirect(std::string redirect)							{_redirect = redirect;}
		void		setKeepalive_time(std::time_t t)							{_keepalive_time = t;}
		void		setKeepalive_timeout(std::time_t t) 						{_keepalive_timeout = t;}
		void		setAllowedMethods(std::set<std::string> m)					{_allowed_methods = m;}
		void		setCGI(bool b)												{_cgi = b;}
		void		setVirtualLocation(bool b)									{_virtualLocation = b;}
		void		setAlias(std::string a)										{_alias = a;}
		void		setErrorPages(std::map<int, std::string> ep)				{_error_pages = ep;}
		void		setErrorPage(int code, std::string page)					{_error_pages.insert(std::make_pair(code, page));}

		ConfigContext(void);
		ConfigContext(ContextType t);
		ConfigContext(const ConfigContext &src);
		ConfigContext& operator=(const ConfigContext &rhs);
		~ConfigContext(void);
};
