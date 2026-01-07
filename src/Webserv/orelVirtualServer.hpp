#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include <map>
#include <string>

class VirtualServer
{
	private:
		void	_initDefaultErrorPages(void);


	public:
		VirtualServer	( void );
		VirtualServer	( const VirtualServer& src );
		VirtualServer&	operator= ( const VirtualServer& rhs );
		~VirtualServer	( void );

		void	initDefaultConfig(void);

		std::map<int, std::string>	error_pages;
};

#endif // VIRTUALSERVER_HPP
