#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include <string>
#include <vector>
#include <map>

struct Location {
	std::string		root;
	std::string		alias;
};

class VirtualServer
{
	private:
		unsigned int	_port;
		std::string		_root;
		std::string		_server_name;

		std::map<std::string, Location>	_locations;

	public:
		VirtualServer	( void );
		VirtualServer	( const VirtualServer& src );
		VirtualServer&	operator= ( const VirtualServer& rhs );
		~VirtualServer	( void );

		const std::map<std::string, Location>	&getLocations(void) const;
		void									setLocations(const std::map<std::string, Location> &locs);
		void								 	setLocationsAt(std::string key, Location &loc);
		const Location							&getLocationAt(std::string key) const;

		void			setPort(unsigned int port);
		unsigned int	getPort(void) const;
		void			setRoot(std::string root);
		std::string		getRoot(void) const;
		void			setServName(std::string name);
		std::string		getServName(void) const;

};

#endif // VIRTUALSERVER_HPP
