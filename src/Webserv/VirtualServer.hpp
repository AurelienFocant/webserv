#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include <string>
#include <vector>
#include <map>

#include "RequestHandler.hpp"

// struct Location {
// 	std::string		root;
// 	std::string		alias;
// };

class VirtualServer
{
	private:
		unsigned int	_port;
		std::string		_root;
	 	std::string		_server_name;
		bool			_autoindex;
		std::vector<std::string>		_indexes;

		std::map<std::string, Location>	_locations;

		void	_initDefaultErrorPages(void);

	public:
		void	initDefaultConfig(void);
		std::map<int, std::string>	error_pages;

		const std::map<std::string, Location>	&getLocations(void) const;
		void									setLocations(const std::map<std::string, Location> &locs);
		void								 	setLocationsAt(std::string key, Location &loc);
		const Location							&getLocationAt(std::string key) const;

		// getReferences or getValues ??
		void			setPort(unsigned int port);
		unsigned int	getPort(void) const;
		void			setRoot(std::string root);
		std::string		getRoot(void) const;
		void			setServName(std::string name);
		std::string		getServName(void) const;
		void							setIndexes(std::vector<std::string> const& src);
		std::vector<std::string> const&	getIndexes(void) const;


		VirtualServer	( void );
		VirtualServer	( const VirtualServer& src );
		VirtualServer&	operator= ( const VirtualServer& rhs );
		~VirtualServer	( void );
};

#endif // VIRTUALSERVER_HPP
