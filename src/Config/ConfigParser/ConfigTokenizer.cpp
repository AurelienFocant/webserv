#include "ConfigTokenizer.hpp"

#include <cstddef>
#include <string>
#include <vector>

ConfigTokenizer::ConfigTokenizer(void)
	: _input("")
	, _pos(0)
	, _line(1)
{
}

ConfigTokenizer::ConfigTokenizer(const std::string& s)
	: _input(s)
	, _pos(0)
	, _line(1)
{
}

ConfigTokenizer::ConfigTokenizer(const ConfigTokenizer& src)
	: _input(src._input)
	, _pos(src._pos)
	, _line(src._line)
{
}

ConfigTokenizer& ConfigTokenizer::operator=(const ConfigTokenizer& rhs)
{
	if (this != &rhs)
	{
		_input = rhs._input;
		_pos   = rhs._pos;
		_line  = rhs._line;
	}
	return *this;
}

ConfigTokenizer::~ConfigTokenizer(void)
{
}

bool ConfigTokenizer::isAtEnd() const
{
	return _pos >= _input.size();
}

char ConfigTokenizer::peek() const
{
	return isAtEnd() ? '\0' : _input[_pos];
}

char ConfigTokenizer::advance()
{
	return isAtEnd() ? '\0' : _input[_pos++];
}

bool ConfigTokenizer::isDelimiter(char c) const
{
	return c == ' ' || c == '\t' || c == '\v' || c == '\n'
	    || c == ';' || c == '{' || c == '}';
}

t_Token ConfigTokenizer::_buildToken(e_tokenType type, const std::string& lexeme)
{
	t_Token token;
	token.tkType   = type;
	token.lexeme = lexeme;
	token.line   = _line;
	return token;
}

void	ConfigTokenizer::scanTokens(void)
{
	while (!isAtEnd())
	{
		char c = advance();
		switch (c)
		{
			case '\n':
				_line++;
				break;
			case ' ':
			case '\t':
			case '\v':
				break;
			case '{':
				_tokenVec.push_back(_buildToken(LBRACE, ""));
				break;
			case '}':
				_tokenVec.push_back(_buildToken(RBRACE, ""));
				break;
			case ';':
				_tokenVec.push_back(_buildToken(SEMICOLON, ""));
				break;
			default: {
				size_t start = _pos - 1;
				while (!isAtEnd() && !isDelimiter(peek()))
					advance();
				_tokenVec.push_back(_buildToken(WORD, _input.substr(start, _pos - start)));
				break;
			}
		}
	}
}

std::vector<t_Token>	ConfigTokenizer::getTokenVec(void)
{
	return (_tokenVec);
}
