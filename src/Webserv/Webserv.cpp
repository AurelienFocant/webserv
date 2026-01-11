#include "Webserv.hpp"
#include "ConfigNode.hpp"
#include "ConfigParser.hpp"
#include "ConfigBuilder.hpp"

#include <iostream>
#include <fstream>

Webserv::Webserv( void )
	: _configPath(defaultConfigPath)
{
}

Webserv::Webserv(char* configPath)
	: _configPath(defaultConfigPath)
{
	if (configPath) {
		_configPath = configPath;
	}
}

Webserv::Webserv( const Webserv& src )
{
	_configPath = src._configPath;
}

Webserv&	Webserv::operator=( const Webserv& rhs )
{
	if (this != &rhs) {
		_configPath = rhs._configPath;
	}
	return (*this);
}

Webserv::~Webserv( void )
{
	std::cout << "Webserv Object Destroyed" << std::endl;
}

void	Webserv::_openConfig(void)
{
	_configFile.open(_configPath.c_str());
	if (!_configFile.is_open())
		throw (std::runtime_error("Couldn't open config file"));
}

static std::string	_fileToString(std::ifstream & file)
{
	std::string	config;
	std::string	line;
	for (; std::getline(file, line); ) {
		config += line;
		config += "\n";
	}
	return (config);
}


void	Webserv::_parseConfig(void)
{
	ConfigTokenizer	tokenizer(_fileToString(_configFile));
	tokenizer.scanTokens();

	ConfigParser	parser(tokenizer.getTokenVec());
	parser.parseConfig();

	// both tokenizer and parser are constructed with what they need,
	// but builder takes it as argument to main fct ?
	ConfigBuilder	builder;
	_servers = builder.build(parser.getRoot());
}

void	Webserv::readConfig()
{
	_openConfig();
	_parseConfig();
}

void	Webserv::initWebServer()
{
	// create epoll fd

	// iterate through virtual servers and create listen fd;

	// modify fd ?? fcntl ?

	// add fd to epoll ?

	// already connection structs that would have the right handler in them ?
}

std::vector<VirtualServer>	Webserv::getServers(void)
{
	return (_servers);
}
