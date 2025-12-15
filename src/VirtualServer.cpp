#include "VirtualServer.hpp"

#include <iostream>

VirtualServer::VirtualServer( void )
{
}

VirtualServer::VirtualServer( const VirtualServer& src )
{
	(void) src;
}

VirtualServer&	VirtualServer::operator=( const VirtualServer& rhs )
{
	if (this != &rhs) {
	}
	return (*this);
}

VirtualServer::~VirtualServer( void )
{
	std::cout << "VirtualServer Object Destroyed" << std::endl;
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
