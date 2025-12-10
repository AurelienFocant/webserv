#include "HTTPTokenizer.hpp"

HTTPTokenizer::HTTPTokenizer() : Tokenizer() {
	std::cout << "Defaul constructor called: HTTPTokenizer" << std::endl;
}

HTTPTokenizer::HTTPTokenizer(std::string const& input) : Tokenizer(input) {
	std::cout << "String constructor called: HTTPTokenizer" << std::endl;
};

std::vector<t_Token>	HTTPTokenizer::scanTokens() {
	std::vector<t_Token> token_list;
	t_Token	new_token;

	//Tokenizing first line of request
	while (_it != _input.end() && *_it != '\n') {
		switch (peek()) {
			case (' '):
				advance();
				break ;
			case ('\r'):
				advance();
				if (peek() == '\n') {
					new_token._tkType = EOL;
					new_token._lexeme = "\\r\\n";
					token_list.push_back(new_token);
				}
				else {
					new_token._tkType = ERROR;
					new_token._lexeme = "ERROR";
					token_list.push_back(new_token);
					return (token_list);
				}
				break ;
			default:
				new_token._tkType = WORD;
				new_token._lexeme = getWord(" \r");
				token_list.push_back(new_token);
		}
	}
	if (_it != _input.end())
		advance();

	int	nbr_eol = 0;
	//Tokenizing Header
	while (_it != _input.end()) {
		switch (peek()) {
			case (':'):
				nbr_eol = 0;
				new_token._tkType = COLON;
				new_token._lexeme = ":";
				token_list.push_back(new_token);
				advance();
				break ;
			case (','):
				nbr_eol = 0;
				new_token._tkType = COMA;
				new_token._lexeme = ",";
				token_list.push_back(new_token);
				advance();
				break ;
			case ('\r'):
				advance();
				if (peek() == '\n') {
					nbr_eol++;
					new_token._tkType = EOL;
					new_token._lexeme = "\\r\\n";
					token_list.push_back(new_token);
					advance();
				}
				else {
					new_token._tkType = ERROR;
					new_token._lexeme = "ERROR";
					token_list.push_back(new_token);
					return (token_list);
				}
				break ;
			default:
				nbr_eol = 0;
				new_token._tkType = WORD;
				new_token._lexeme = getWord(":,\r");
				token_list.push_back(new_token);
		}
		if (nbr_eol == 2)
			break ;
	}

	//Tokenizing Body
	if (_it != _input.end()) {
		new_token._tkType = WORD;
		new_token._lexeme = getWord("");
		token_list.push_back(new_token);
	}
	return (token_list);
}

std::string	HTTPTokenizer::getWord(std::string delim_list) {
	while (*_it == ' ' || *_it == '\t')
		advance();
	std::string::const_iterator	end_word = _it;
	while (end_word != _input.end() && delim_list.find(*end_word) == std::numeric_limits<unsigned long>::max())
		end_word++;
	std::string	word = _input.substr(_it - _input.begin(), end_word - _it);
	_it = end_word;
	while (*(word.end() - 1) == ' ')
		word.erase(word.size() - 1);
	return (word);
}
