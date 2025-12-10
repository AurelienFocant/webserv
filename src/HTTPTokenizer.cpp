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
	while (*_it != '\n') {
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
	advance();

	//Tokenizing Header
	while (_it != _input.end()) {
		switch (peek()) {
			case (':'):
				new_token._tkType = COLON;
				new_token._lexeme = ":";
				token_list.push_back(new_token);
				advance();
				break ;
			case (','):
				new_token._tkType = COMA;
				new_token._lexeme = ",";
				token_list.push_back(new_token);
				advance();
				break ;
			case ('\r'):
				advance();
				if (peek() == '\n') {
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
				new_token._tkType = WORD;
				new_token._lexeme = getWord(":,\r");
				token_list.push_back(new_token);
		}
	}
	return (token_list);
}

std::string	HTTPTokenizer::getWord(std::string delim_list) {
	int		j = 0;
	while (_it[j] == ' ' || _it[j] == '\t')
		j++;
	int		i = 0;
	while (delim_list.find(_it[i + j]) == -1)
		i++;
	std::string	word = _input.substr(_it - _input.begin() + j, i);
	if (_input[_it - _input.begin() + i + j])
		_it += i + j;
	while (*(word.end() - 1) == ' ')
		word.erase(word.size() - 1);
	return (word);
}
