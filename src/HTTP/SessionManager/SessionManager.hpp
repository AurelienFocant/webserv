#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <map>
#include <string>
#include <ctime>
#include <sstream>


class SessionManager 
{
	public:

	SessionManager();
	~SessionManager();

	SessionManager&				createManager();
	std::string					createSession();
	bool						exists(const std::string& id) const;

	void						incrementValue(const std::string& id);
	int							getValue(const std::string& id) const;

	private: 

	std::string					_generateId();

	std::map<std::string, int>	_sessions;
};

#endif