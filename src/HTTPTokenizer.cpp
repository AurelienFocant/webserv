#include "HTTPTokenizer.hpp"

HTTPTokenizer::HTTPTokenizer(std::string input) : Tokenizer(input) {
};

std::vector<t_Token>	HTTPTokenizer::scanTokens() {
	std::vector<t_Token> token_list;
	t_Token	new_token;
	std::stringstream	stream;
	stream << _input;
	int	i = 3;
	while (i) {
		new_token._tkType = WORD; 
		stream >> new_token._lexeme; 
		token_list.push_back(new_token);
		i--;
	}
	_input.erase(0, _input.find('\n') + 1);
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
				new_token._lexeme = getWord();
				token_list.push_back(new_token);
		}
	}
	return (token_list);
}

std::string	HTTPTokenizer::getWord() {
	int		j = 0;
	while (_it[j] == ' ' || _it[j] == '\t')
		j++;
	int		i = 0;
	while (_it[i + j] != ':' && _it[i + j] != ',' && _it[i + j] != '\r')
		i++;
//	while (_it[i] == ' ' || _it[i] == '\t')
//		i--;
	std::string	word = _input.substr(_it - _input.begin() + j, i);
	if (_input[_it - _input.begin() + i + j])
		_it += i + j;
	while (*(word.end() - 1) == ' ')
		word.erase(word.size() - 1);
	return (word);
}
