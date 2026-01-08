#include "HTTPTokenizer.hpp"

HTTPTokenizer::HTTPTokenizer() : Tokenizer() {
	_tokenizing = 0;
	std::cout << "Defaul constructor called: HTTPTokenizer" << std::endl;
}

HTTPTokenizer::HTTPTokenizer(std::string const& input) : Tokenizer(input) {
	_tokenizing = 0;
	std::cout << "String constructor called: HTTPTokenizer" << std::endl;
}

HTTPTokenizer::~HTTPTokenizer() {
	std::cout << "Destructor called: HTTPTokenizer" << std::endl;
}

std::vector<t_Token>	HTTPTokenizer::scanTokens() {
	t_Token	new_token;

	//Tokenizing first line of request
	int	nbr_eol = 0;
	while (_tokenizing < 4 && _it != _input.end() && nbr_eol == 0) {
		switch (*_it) {
			case (' '):
				_input.erase(0, 1);
				break ;
			case ('\r'):
				_it++;
				if (*_it == '\n') {
					new_token.tkType = EOL;
					new_token.lexeme = "\\r\\n";
					_token_list.push_back(new_token);
					_it -= 1;
					_input.erase(0, 2);
					nbr_eol = 1;
				}
				else {
					new_token.tkType = ERROR;
					new_token.lexeme = "ERROR";
					_token_list.push_back(new_token);
					_it -= 1;
					_input.erase(0, 1);
					return (_token_list);
				}
				break ;
			default:
				new_token.tkType = WORD;
				new_token.lexeme = getWord(" \r");
				_token_list.push_back(new_token);
				_tokenizing++;
		}
	}
	nbr_eol = 0;
	//Tokenizing Header
	while (_it != _input.end() && nbr_eol != 2) {
		switch (*_it) {
			case (':'):
				nbr_eol = 0;
				new_token.tkType = COLON;
				new_token.lexeme = ":";
				_token_list.push_back(new_token);
				_input.erase(0, 1);
				break ;
			case (','):
				nbr_eol = 0;
				new_token.tkType = COMA;
				new_token.lexeme = ",";
				_token_list.push_back(new_token);
				_input.erase(0, 1);
				break ;
			case ('\r'):
				advance();
				if (peek() == '\n') {
					nbr_eol++;
					new_token.tkType = EOL;
					new_token.lexeme = "\\r\\n";
					_token_list.push_back(new_token);
					_it -= 1;
					_input.erase(0, 2);
				}
				else {
					new_token.tkType = ERROR;
					new_token.lexeme = "ERROR";
					_token_list.push_back(new_token);
					_it -= 1;
					_input.erase(0, 1);
					return (_token_list);
				}
				break ;
			default:
				nbr_eol = 0;
				new_token.tkType = WORD;
				if (_token_list.back().tkType == COLON || _token_list.back().tkType == COMA)
					new_token.lexeme = getWord(",\r");
				else
					new_token.lexeme = getWord(":,\r");
				_token_list.push_back(new_token);
		}
	}
//	if (_it == _input.end() || nbr_eol == 2)
//		_progress++;
/*
	//Tokenizing Body
	if (_it != _input.end()) {
		new_token.tkType = WORD;
		new_token.lexeme = getWord("");
		_token_list.push_back(new_token);
	}
*/
	if (_it == _input.end()) {
		new_token.tkType = EOC;
		new_token.lexeme = "\0";
		_token_list.push_back(new_token);
	}
	_list_it = _token_list.begin();
	return (_token_list);
}

std::string	HTTPTokenizer::getWord(std::string delim_list) {
	//std::string::const_iterator	it_start = _it;
	while (*_it == ' ' || *_it == '\t') {
		_input.erase(0, 1);
//		advance();
	}
	std::string::const_iterator	end_word = _it;
	while (end_word != _input.end() && delim_list.find(*end_word) == std::numeric_limits<unsigned long>::max())
		end_word++;
	std::string	word = _input.substr(_it - _input.begin(), end_word - _it);
	_input.erase(_it - _input.begin(), word.size());
	while (*(word.end() - 1) == ' ')
		word.erase(word.size() - 1);
	return (word);
}

std::string	HTTPTokenizer::getInput() const {
	return (_input);
}

std::string	HTTPTokenizer::extractInput(size_t len) {
	std::string	tmp = _input.substr(0, len);
	_input.erase(0, len);
	return (tmp);
}

std::string	HTTPTokenizer::extractInput(char character) {
	size_t	pos	= _input.find(character);
	std::string	tmp = _input.substr(0, pos);
	_input.erase(0, pos + 1);
	return (tmp);
}

void	HTTPTokenizer::cleanTokenList() {
	_token_list.clear();
	_tokenizing = 0;
}

bool	HTTPTokenizer::addInput(std::string input) {
	_input += input;
	_it = _input.begin();
	return (true);
}

std::vector<t_Token>	HTTPTokenizer::getTokenList() const {
	return (_token_list);
}

void	HTTPTokenizer::removeEOC() {
	int	pos = _list_it - _token_list.begin();
	if (_token_list.back().tkType == EOC)
		_token_list.pop_back();
	_list_it = _token_list.begin() + pos;
}

std::string	HTTPTokenizer::getTokenType(t_Token token) {
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
		case (EOC):
			return ("EOC");	
		default:
	return ("\0");
	}
}
