#include "Token.hpp"

int	Token::setType(std::string& type) {
	type = "Hello";
	content = type;
	return (0);
}

Token&	Token::toTokenList(std::string	request) {
	std::string			content;
	std::stringstream	stream;

	stream << request;
	stream >> content;
/*	if (!token)
		return (NULL);
*/
	Token*	token = new Token();
	token->setType(content);
	return (*token); 
}
