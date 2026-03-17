/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <stempels@student.42belgium.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 14:31:13 by stempels          #+#    #+#             */
/*   Updated: 2026/03/17 14:31:15 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tokenizer.hpp"

#include <iostream>

Tokenizer::Tokenizer( void )
{
}

Tokenizer::Tokenizer( std::string const& s )
	: _input(s)
	, _it(_input.begin())
{
}

Tokenizer::Tokenizer( const Tokenizer& src )
	: _input(src._input)
	, _it(src._it)
{
}

Tokenizer&	Tokenizer::operator=( const Tokenizer& rhs )
{
	if (this != &rhs) {
		//DO NOTHING
	}
	return (*this);
}

Tokenizer::~Tokenizer( void )
{
}

char	Tokenizer::peek() const
{
	if (_it + 1 == _input.end())
		return ('\0');
	return (*(_it + 1));
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
	switch (token.tkType) {
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
		default:
			return ("\0");
	}
}
