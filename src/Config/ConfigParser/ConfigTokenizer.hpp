#define CONFIGTOKENIZER_HPP

#include <string>
#include <vector>

enum e_tokenType {
	ERROR, WORD, SEMICOLON, LBRACE, RBRACE
};

typedef struct {
	e_tokenType		type;
	std::string		lexeme;
	unsigned int	line;
}	t_Token;

class ConfigTokenizer
{
	private:
		std::vector<t_Token>	_tokenVec;

		std::string _input;
		size_t      _pos;
		size_t      _line;

		bool        isAtEnd() const;
		char        peek() const;
		char        advance();

		t_Token     _buildToken(e_tokenType type, const std::string& lexeme);

		bool        isDelimiter(char c) const;

	public:
		void	scanTokens(void);
		std::vector<t_Token>	getTokenVec(void);

		ConfigTokenizer(void);
		ConfigTokenizer(const std::string& s);
		ConfigTokenizer(const ConfigTokenizer& src);
		ConfigTokenizer& operator=(const ConfigTokenizer& rhs);
		~ConfigTokenizer(void);
};
