#ifndef		TOKEN_HPP
# define	TOKEN_HPP

# include <string>
# include <sstream>
# include <iostream>

class	Token {
	public:

	static Token&	toTokenList(std::string request) ;
	/*Setter - Getter*/
		int	setType(std::string& type);

	private:
		std::string	type;
		std::string	content;
};

#endif
