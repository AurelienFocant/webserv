
#include "CppEnv.hpp"

/*Constructors*/
//Initialise an empty environement
CppEnv::CppEnv() {}

//Construct the environement based on the request data
CppEnv::CppEnv(const Request& data) {
	setEnv(data);
}

/*Copy Constructor*/
CppEnv::CppEnv(const CppEnv& copy_from) {
	if (this != &copy_from) {
		this->_env = copy_from.getCppEnv();
	}
}

/*Destructor*/
CppEnv::~CppEnv() {}

/*Overloaded Operator*/
CppEnv&	CppEnv::operator=(const CppEnv& rhs) {
	if (this != &rhs) {
		this->_env = rhs.getEnv();
	}
	return (*this);
}

/*Public Methods*/
bool	CppEnv::addEnvParameter(const std::string& parameter) {
	std::string::iterator pos = paremeter.find('=');
	if (pos == parameter.end())
		return (false);
	const std::string	parameter_name = parameter.copy(0, pos);
	const std::string	parameter_value = parameter.copy(pos + 1);
	if (!addEnvParameter(parameter_name, parameter_value))
		return (false);
	return (true);
}

bool	CppEnv::addEnvParameter(const std::string parameter_name, const std::string parameter_value) {
	if (_env.insert(std::make_pair(parameter_name, parameter_value))->second == false)
		return (false);
	return (true);
}

char**	CppEnv::convertToCEnv() const {
	char**	c_env[env.size() + 1];	

	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = _env.begin(); it != _env.end(); it++) {
		c_env[i] = (it.first + '=' + it.second).c_str();
		i++;
	}
	c_env[i] = NULL;
	return (c_env);
}

/*Getters*/
std::map<std::string, std::string>			getCppEnv() const {
	return (_env);
}

const std::map<std::string, std::string>	getCppEnv() const {
	return (_env);
}

/*Private Methods*/
void	CppEnv::setEnv(const Request& data) {
	std::multimap<std::string, std::string> tmp = data.getHeaders();
	for (std::multimap<std::string, std::string>::const_iterator it = tmp.begin(); it != tmp.end(); it++) {
		if (!addEnvParameter(it->first, it->second))
			continue;
	}
	return ;
}
