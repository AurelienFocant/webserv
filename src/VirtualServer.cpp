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

void	VirtualServer::initDefaultConfig(void)
{
	this->error_pages[404] = "./www/error_pages/404.html";
}
