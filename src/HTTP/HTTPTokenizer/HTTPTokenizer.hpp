#ifndef		HTTPTOKENIZER_HPP
# define	HTTPTOKENIZER_HPP

# include <string>
# include <limits>
# include <iostream>
# include <sstream>
# include "Tokenizer.hpp"

class	HTTPTokenizer : private Tokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		HTTPTokenizer();
		HTTPTokenizer(std::string const& input);
		virtual ~HTTPTokenizer();

	/*Overloaded Operators*/
	/*Public Methods*/
		std::vector<t_Token>	scanTokens() ;
		std::string				getWord(std::string delim_list) ;
		std::string				extractInput(size_t len) ;
		std::string				extractInput(char character) ;
		bool					addInput(std::string input) ;
		void					cleanTokenList() ;
		void					removeEOC();

	/*Getters*/
		std::string				getInput() const ;
		std::vector<t_Token>	getTokenList() const;
		
	/*Static*/
		static std::string	getTokenType(t_Token token) ;

	protected:
		std::vector<t_Token>					_token_list;
		std::vector<t_Token>::const_iterator	_list_it;
		
	private:
	/*Private Attributes*/
		int										_tokenizing;
		int										_nbr_eol;
};

#endif
