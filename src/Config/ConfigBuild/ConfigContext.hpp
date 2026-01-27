#pragma once
#include <string>
#include <map>
#include "VirtualServer.hpp"

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
		std::string _serverName;
		std::string _locationName;
		std::map<std::string, Location> _locations;
		std::vector<std::string>		_indexes;
		bool		_autoindex;

	public:
		ConfigContext(void);
		ConfigContext(ContextType t);
		ConfigContext(const ConfigContext &src);
		ConfigContext& operator=(const ConfigContext &rhs);
		~ConfigContext(void);

		void inheritFrom(const ConfigContext &parent);

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
		void		setAutoindex(bool b);

};
