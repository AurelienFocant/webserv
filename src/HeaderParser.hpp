#ifndef		HEADERTOKENIZER_HPP
# define	HEADERTOKENIZER_HPP

# include "tokenizer.hpp"

class	HeaderTokenizer : virtual public Tokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		HeaderTokenizer();
	/*Overloaded Operators*/
	/*Public Methods*/
		std::vector<t_Token>	scanTokens() ;
		void					addToken() ;
		
	private:
	/*Private Attributes*/
		std::vector<t_Token>	_token_list;
};
