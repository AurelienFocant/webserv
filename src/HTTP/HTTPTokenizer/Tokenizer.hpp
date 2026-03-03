#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>

typedef enum e_tokenType
{
	ERROR, WORD, COLON, COMA, EOL, SEMICOLON, LBRACE, RBRACE

}	t_tokenType;

typedef struct
{
	t_tokenType		tkType;
	std::string		lexeme;
	unsigned int	line;
}	t_Token;

class Tokenizer
{
	protected:
		std::string					_input;
		std::string::const_iterator _it;
		unsigned int				_current_line;

		// char	match	();

	public:
		Tokenizer	( void );
		Tokenizer	( std::string const& s );

		Tokenizer	( const Tokenizer& src );
		Tokenizer&	operator= ( const Tokenizer& rhs );
		virtual ~Tokenizer	( void );

		char	peek() const ;
		char	advance() ;
		bool	match(char c) ;

		bool				setInput(std::string input) ;
		virtual std::string	getInput() const = 0 ;

		virtual std::vector<t_Token>	scanTokens() = 0 ;
		static std::string				getTokenType(t_Token token) ;
};

#endif
