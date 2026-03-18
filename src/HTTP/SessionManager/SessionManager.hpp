#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include "Request.hpp"
#include "Response.hpp"

#include <map>
#include <string>
#include <ctime>
#include <sstream>

static const int			session_lifetime = 60;

struct	cookies
{
	int						count;
	std::time_t				created_at;

	cookies() : count(0), created_at(std::time(NULL)) {};
};

class SessionManager 
{
	public:

	static SessionManager&		createManager();
	std::string					extractCookies(const Request& request, const std::string& id);
	std::string					handleId(std::string id);

	void						incrementValue(const std::string& id, Response& response);
	int							getValue(const std::string& id) const;
	cookies						getCookie(const std::string& id) const;
	bool						deleteCookie(const std::string& id);

	private: 

	SessionManager();

	std::string					_createSession();
	std::string					_generateId();
	bool						exists(const std::string& id) const;

	std::map<std::string, cookies>	_sessions;
};

#endif