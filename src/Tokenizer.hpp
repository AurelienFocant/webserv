#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>

typedef enum {
	WORD, SEMICOLON
}	t_tokenType;

typedef struct {
	t_tokenType	_tkType;
	std::string	_lexeme;
}	t_Token;

class Tokenizer
{
	private:

		std::string					_input;
		std::string::const_iterator _it;
		unsigned int				_current_line;

		// char	match	();


	public:
		Tokenizer	( void );
		Tokenizer	( std::string const& s );

		Tokenizer	( const Tokenizer& src );
		Tokenizer&	operator= ( const Tokenizer& rhs );
		~Tokenizer	( void );

		char	peek() const;
		char	advance();
		bool	match(char c);

		virtual std::vector<t_Token>	scanTokens() = 0;
};

#endif // TOKENIZER_HPP
