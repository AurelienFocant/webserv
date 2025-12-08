#ifndef		HTTPTOKENIZER_HPP
# define	HTTPTOKENIZER_HPP

# include <string>
# include <iostream>
# include <sstream>
# include "Tokenizer.hpp"

class	HTTPTokenizer : virtual public Tokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		HTTPTokenizer(std::string input);
	/*Overloaded Operators*/
	/*Public Methods*/
		std::vector<t_Token>	scanTokens() ;
		std::string				getWord() ;
		
	private:
	/*Private Attributes*/
		std::vector<t_Token>	_token_list;
};

#endif
