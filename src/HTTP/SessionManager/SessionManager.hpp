#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include "Request.hpp"
#include "Response.hpp"

#include <map>
#include <string>
#include <ctime>
#include <sstream>

class SessionManager 
{
	public:

	static SessionManager&		createManager();
	std::string					extractCookies(const Request& request, const std::string& id);
	std::string					handleId(std::string id);

	void						incrementValue(const std::string& id, Response& response);
	int							getValue(const std::string& id) const;

	private: 

	SessionManager();

	std::string					_createSession();
	std::string					_generateId();
	bool						exists(const std::string& id) const;

	std::map<std::string, int>	_sessions;
};

#endif