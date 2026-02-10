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


		ContextType getType(void) const;
		int         getPort(void) const;
		void        setPort(int port);
		std::string getRoot(void) const;
		void        setRoot(const std::string &root);
		std::string getServerName(void) const;
		void        setServerName(const std::string &serverName);
		std::string getLocationName(void) const;
		void		setLocationName(const std::string &locationName);
		const std::map<std::string, Location>&	getLocations(void) const;
		void									addLocation(const std::string &name, const Location &loc);
		void							setIndexes(std::vector<std::string> const& src);
		std::vector<std::string> const&	getIndexes(void) const;
		bool		getAutoindex(void) const;
		void		setAutoindex(bool b)		{_autoindex = b;}
		int			getRedirectCode(void) const	{return _redirect_code;}
		void		setRedirectCode(int code)	{_redirect_code = code;}
		std::string	getRedirect(void) const		{return _redirect;}
		void		setRedirect(std::string redirect)	{_redirect = redirect;}
		std::time_t	getKeepalive_time() const			{return _keepalive_time;}
		void		setKeepalive_time(std::time_t t)	{_keepalive_time = t;}
		std::time_t	getKeepalive_timeout() const		{return _keepalive_timeout;}
		void		setKeepalive_timeout(std::time_t t) {_keepalive_timeout = t;}
		std::set<std::string>	getAllowedMethods(void) const	{return _allowed_methods;}
		void					setAllowedMethods(std::set<std::string> m) {_allowed_methods = m;}
		void		setCGI(bool b) {_cgi = b;}
		bool		getCGI() const {return _cgi;}
		void		setVirtualLocation(bool b) {_virtualLocation = b;}
		bool		getVirtualLocation() const {return _virtualLocation;}
		void		setAlias(std::string a) {_alias = a;}
		std::string	getAlias() const {return _alias;}
		void		setErrorPages(std::map<int, std::string> ep) {_error_pages = ep;}
		std::map<int, std::string>	getErrorPages() const {return _error_pages;}
		void		setErrorPage(int code, std::string page) {_error_pages.insert(std::make_pair(code, page));}
		std::string	getErrorPage(int code) const {return _error_pages.at(code);}

		ConfigContext(void);
		ConfigContext(ContextType t);
		ConfigContext(const ConfigContext &src);
		ConfigContext& operator=(const ConfigContext &rhs);
		~ConfigContext(void);
};
