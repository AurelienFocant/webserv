#include "SessionManager.hpp"


SessionManager::SessionManager() {}

SessionManager::~SessionManager() {}

SessionManager&	SessionManager::createManager()
{
	static SessionManager manager;
	return manager;
}

std::string	SessionManager::createSession()
{
	std::string id = _generateId();
	_sessions[id] = 0;
	return id;
}

std::string	SessionManager::_generateId()
{
	bool init = false;

	if (!init)
	{
		std::srand(std::time(NULL));
		init = true;
	}

	std::stringstream ss;
	for (int i = 0; i < 6; i++)
		ss << std::rand();
	return ss.str();
}

bool	SessionManager::exists(const std::string& id) const
{
	return _sessions.find(id) != _sessions.end();
}

void	SessionManager::incrementValue(const std::string& id)
{
	_sessions[id] += 1;
}

int		SessionManager::getValue(const std::string& id) const
{
	std::map<std::string, int>::const_iterator it = _sessions.find(id);
	if (it == _sessions.end())
		return 0;
	return it->second;
}