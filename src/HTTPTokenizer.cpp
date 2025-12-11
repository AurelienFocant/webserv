#include "HTTPTokenizer.hpp"

HTTPTokenizer::HTTPTokenizer() : Tokenizer() {
	std::cout << "Defaul constructor called: HTTPTokenizer" << std::endl;
}

HTTPTokenizer::HTTPTokenizer(std::string const& input) : Tokenizer(input) {
	std::cout << "String constructor called: HTTPTokenizer" << std::endl;
};

std::vector<t_Token>	HTTPTokenizer::scanTokens() {
	t_Token	new_token;

	//Tokenizing first line of request
	while (_it != _input.end() && *_it != '\n') {
		switch (*_it) {
			case (' '):
				_input.erase(0, 1);
				_it = _input.begin();
				break ;
			case ('\r'):
				_it++;
				if (*_it == '\n') {
					new_token._tkType = EOL;
					new_token._lexeme = "\\r\\n";
					_token_list.push_back(new_token);
					_input.erase(0, 2);
					_it = _input.begin();
				}
				else {
					new_token._tkType = ERROR;
					new_token._lexeme = "ERROR";
					_token_list.push_back(new_token);
					_input.erase(0, 1);
					_it = _input.begin();
					return (_token_list);
				}
				break ;
			default:
				new_token._tkType = WORD;
				new_token._lexeme = getWord(" \r");
				_token_list.push_back(new_token);
				_input.erase(0, new_token._lexeme.size());
				_it = _input.begin();
		}
	}
	if (_it != _input.end())
		_it++;

	int	nbr_eol = 0;
	//Tokenizing Header
	while (_it != _input.end()) {
		switch (peek()) {
			case (':'):
				nbr_eol = 0;
				new_token._tkType = COLON;
				new_token._lexeme = ":";
				_token_list.push_back(new_token);
				advance();
				break ;
			case (','):
				nbr_eol = 0;
				new_token._tkType = COMA;
				new_token._lexeme = ",";
				_token_list.push_back(new_token);
				advance();
				break ;
			case ('\r'):
				advance();
				if (peek() == '\n') {
					nbr_eol++;
					new_token._tkType = EOL;
					new_token._lexeme = "\\r\\n";
					_token_list.push_back(new_token);
					advance();
				}
				else {
					new_token._tkType = ERROR;
					new_token._lexeme = "ERROR";
					_token_list.push_back(new_token);
					return (_token_list);
				}
				break ;
			default:
				nbr_eol = 0;
				new_token._tkType = WORD;
				new_token._lexeme = getWord(":,\r");
				_token_list.push_back(new_token);
		}
		if (nbr_eol == 2)
			break ;
	}

	//Tokenizing Body
	if (_it != _input.end()) {
		new_token._tkType = WORD;
		new_token._lexeme = getWord("");
		_token_list.push_back(new_token);
	}
	return (_token_list);
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

std::string	HTTPTokenizer::getInput() const {
	return (_input);
}

void	HTTPTokenizer::cleanTokenList() {
	_token_list.clear();
}

bool	HTTPTokenizer::setInput(std::string input) {
	_input.clear();
	_input = input;
	_it = _input.begin();
	return (true);
}

std::vector<t_Token>	HTTPTokenizer::getTokenList() const {
	return (_token_list);
}

std::string	HTTPTokenizer::getTokenType(t_Token token) {
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
