#include "SessionManager.hpp"


SessionManager::SessionManager() {}

SessionManager&	SessionManager::createManager()
{
	static SessionManager manager;
	return manager;
}

std::string	SessionManager::handleId(std::string id)
{
	if (!id.empty() && exists(id))
		return id;
	return _createSession();
}


std::string	SessionManager::_createSession()
{
	std::string id = _generateId();
	_sessions[id].count = 0;
	return id;
}

std::string	SessionManager::_generateId()
{
	static bool init = false;

	if (!init)
	{
		std::srand(std::time(NULL));
		init = true;
	}

	std::stringstream ss;
	for (int i = 0; i < 4; i++)
		ss << std::rand();
	return ss.str();
}

bool	SessionManager::exists(const std::string& id) const
{
	return _sessions.find(id) != _sessions.end();
}

void	SessionManager::incrementValue(const std::string& id, Response& response)
{
	_sessions[id].count += 1;
	response.setCounterSession(_sessions[id].count);

}

int		SessionManager::getValue(const std::string& id) const
{
	std::map<std::string, cookies>::const_iterator it = _sessions.find(id);
	if (it == _sessions.end())
		return 0;
	return it->second.count;
}

cookies	SessionManager::getCookie(const std::string& id) const
{
	return _sessions.find(id)->second;
}

bool	SessionManager::deleteCookie(const std::string& id)
{
	if (!exists(id))
		return false;
	_sessions.erase(id);
	return true;
}