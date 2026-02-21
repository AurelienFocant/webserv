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
		std::string		_alias;
	 	std::string		_server_name;
		std::string		_redirect;
		int				_redirect_code;
		bool			_autoindex;
		std::time_t		_keepalive_time;
		std::time_t		_keepalive_timeout;

		std::map<int, std::string>		_error_pages;
		std::vector<std::string>		_indexes;
		std::set<std::string>			_allowed_methods;
		std::map<std::string, Location>	_locations;

	public:
		const std::map<std::string, Location>	&getLocations(void) const;
		void									setLocations(const std::map<std::string, Location> &locs);
		void								 	setLocationsAt(std::string key, Location &loc);
		const Location							&getLocationAt(std::string key) const;

		// getReferences or getValues ??
		void								setPort(unsigned int port);
		unsigned int						getPort(void) const;
		void								setRoot(std::string root);
		std::string							getRoot(void) const;
		void								setServName(std::string name);
		std::string							getServName(void) const;
		void								setIndexes(std::vector<std::string> const& src);
		std::vector<std::string> const&		getIndexes(void) const;
		bool								getAutoindex(void) const;
		void	   	 						setAutoindex(bool b);
		void		setAlias(std::string a) {_alias = a;}
		std::string	getAlias() const {return _alias;}
		void		setErrorPages(std::map<int, std::string> ep) {_error_pages = ep;}
		std::map<int, std::string> const&	getErrorPages() const {return _error_pages;}
		void		setErrorPage(int code, std::string page) {_error_pages.insert(std::make_pair(code, page));}
		std::string	getErrorPage(int code) const {return _error_pages.at(code);}

		std::time_t	getKeepaliveTime(void) const	{return _keepalive_time;}
		std::time_t	getKeepaliveTimeout(void) const	{return _keepalive_timeout;}

		VirtualServer	( void );
		VirtualServer	(ConfigContext const& ctxt);
		VirtualServer	( const VirtualServer& src );
		VirtualServer&	operator= ( const VirtualServer& rhs );
		~VirtualServer	( void );
};

#endif // VIRTUALSERVER_HPP
