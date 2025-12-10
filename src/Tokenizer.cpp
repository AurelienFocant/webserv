#include "Tokenizer.hpp"

#include <iostream>

Tokenizer::Tokenizer( void )
	: _input("default input")
	, _it(_input.begin()) {
	std::cout << "Default constructor called: Tokenizer" << std::endl;
}

Tokenizer::Tokenizer( std::string const& s )
	: _input(s)
	, _it(_input.begin()) {
	std::cout << "String constructor called: Tokenizer" << std::endl;
}

Tokenizer::Tokenizer( const Tokenizer& src )
	: _input(src._input)
	, _it(src._it)
{
}

Tokenizer&	Tokenizer::operator=( const Tokenizer& rhs )
{
	if (this != &rhs) {
		// clone input ??
	}
	return (*this);
}

Tokenizer::~Tokenizer( void )
{
	std::cout << "Tokenizer Object Destroyed" << std::endl;
}

char	Tokenizer::peek() const
{
	return (*_it);
}

char	Tokenizer::advance()
{
	return (*(_it++));
}

bool	Tokenizer::match(char c)
{
	if (_it == _input.end() || *_it != c)
		return (false);
	advance();
	return (true);
}

std::string	Tokenizer::getTokenType(t_Token token) {
	switch (token._tkType) {
		case (ERROR):
			return ("ERROR");	
		case (WORD):
			return ("WORD");	
		case (COLON):
			return ("COLON");	
		case (COMA):
			return ("COMA");	
		case (EOL):
			return ("EOL");	
	}
	return ("\0");
}
