
#ifndef		CPPENV_HPP
# define	CPPENV_HPP

# include "Request.hpp"
# include <string>
# include <map>

class	request;

class	CppEnv {
	public:
	/*Constructors - Copy Constructor - Destructor*/
		CppEnv();
		CppEnv(const Request& data);
		cppEnv(const Cppenv& copy_from);
		~CppEnv();

	/*Overloaded Operator*/
		CppEnv&	operator=(const CppEnv& rhs);

	/*Public Methods*/
		bool	addEnvParameter(const std::string& parameter); //parameter: "PARAMETER_NAME=PARAMETER_VALUE"
		bool	addEnvParameter(const std::string parameter_name, const std::string parameter_value);
		char**	convertToCEnv() const;
	
	/*Getters*/
		std::map<std::string, std::string>			getCppEnv() const;
		const std::map<std::string, std::string>&	getCppEnv() const;

	private:
	/*Private Attributes*/
		std::map<std::string, std::string>	_env;

	/*Private Methods*/
		void	setEnv(const Request& data);

};

#endif
