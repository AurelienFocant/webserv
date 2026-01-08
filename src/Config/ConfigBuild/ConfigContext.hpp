#pragma once
#include <string>
#include <map>
#include "VirtualServer.hpp"

enum ContextType {
    MAIN,
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

	public:
		ConfigContext(void);
		ConfigContext(ContextType t);
		ConfigContext(const ConfigContext &src);
		ConfigContext& operator=(const ConfigContext &rhs);
		~ConfigContext(void);

		void inheritFrom(const ConfigContext &parent);

		ContextType getType(void) const;
		int         getPort(void) const;
		std::string getRoot(void) const;
		std::string getServerName(void) const;
		std::string getLocationName(void) const;
		const std::map<std::string, Location> &getLocations(void) const;
		void        setPort(int port);
		void        setRoot(const std::string &root);
		void        setServerName(const std::string &serverName);
		void		setLocationName(const std::string &locationName);
		void        addLocation(const std::string &name, const Location &loc);
};
