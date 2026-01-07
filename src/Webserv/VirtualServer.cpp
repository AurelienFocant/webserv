#include "VirtualServer.hpp"

#include <iostream>

VirtualServer::VirtualServer( void )
	: _port(0)
	, _root("")
	, _server_name("")
	, _locations()
{
}

VirtualServer::VirtualServer(const VirtualServer& src)
	: _port(src._port)
	, _root(src._root)
	, _server_name(src._server_name)
	, _locations(src._locations)
{
}

VirtualServer&	VirtualServer::operator=( const VirtualServer& rhs )
{
	if (this != &rhs) {
		_port = rhs._port;
		_root = rhs._root;
		_server_name = rhs._server_name;
		_locations = rhs._locations;
	}
	return (*this);
}

VirtualServer::~VirtualServer( void )
{
	// free all locations ?
}

const std::map<std::string, Location>& VirtualServer::getLocations() const
{
	return _locations;
}

void VirtualServer::setLocations(const std::map<std::string, Location> &locs)
{
	_locations = locs;
}

const Location& VirtualServer::getLocationAt(std::string key) const
{
	return _locations.at(key);
}

void VirtualServer::setLocationsAt(std::string key, Location & loc)
{
	_locations[key] = loc;
}

void	VirtualServer::setRoot(std::string root)
{
	_root = root;
}

std::string		VirtualServer::getRoot(void) const
{
	return (_root);
}

void	VirtualServer::setPort(unsigned int port)
{
	_port = port;
}

unsigned int	VirtualServer::getPort(void) const
{
	return (_port);
}

void			VirtualServer::setServName(std::string name)
{
	_server_name = name;
}

std::string		VirtualServer::getServName(void) const
{
	return (_server_name);
}

void	VirtualServer::_initDefaultErrorPages(void)
{
	std::string	root = "./data/error_pages/";
	this->error_pages[400] = root + "400.html";
	this->error_pages[404] = root + "404.html";
}

void	VirtualServer::initDefaultConfig(void)
{
	_initDefaultErrorPages();
}
