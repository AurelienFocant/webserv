#include "HTTPTokenizer.hpp"

HTTPTokenizer::HTTPTokenizer() : Tokenizer() {
	cleanTokenList();
}

HTTPTokenizer::HTTPTokenizer(std::string const& input) : Tokenizer(input) {
	cleanTokenList();
}

HTTPTokenizer::HTTPTokenizer(const HTTPTokenizer& src)
	: Tokenizer(src)
	, _token_list(src._token_list)
	, _tokenizing(src._tokenizing)
	, _nbr_eol(src._nbr_eol)
{
    _list_it = _token_list.begin();
}

HTTPTokenizer::~HTTPTokenizer() {
}

std::vector<t_Token>	HTTPTokenizer::scanTokens() {
	t_Token	new_token;
	_it = _input.begin();

	//Tokenizing first line of request
	while (_tokenizing < 4 && _it != _input.end() && _nbr_eol == 0) {
		switch (*_it) {
			case ('\0'):
				break ;
			case ('\r'):
				switch (peek()) {
					case ('\n'):
						new_token.tkType = EOL;
						new_token.lexeme = "\\r\\n";
						_token_list.push_back(new_token);
						_tokenizing = 4;
						_nbr_eol = 1;
						_it = _it + 2;
						break ;
					case ('\0'):
						break ;
					default: 
						new_token.tkType = ERROR;
						new_token.lexeme = "ERROR";
						_token_list.push_back(new_token);
						return (_token_list);
				}
				break ;
			default:
				new_token.tkType = WORD;
				new_token.lexeme = getWord(" \r");
				if (!new_token.lexeme.empty()) {
					_token_list.push_back(new_token);
					_tokenizing++;
				}
		}
		if (_it == _input.begin())
			break ;
		_input.erase(0, _it - _input.begin());
		_it = _input.begin();
	}
	//Tokenizing Header
	_it = _input.begin();
	while (_it != _input.end() && _nbr_eol < 2) {
		switch (*_it) {
			case ('\0'):
				break ;
			case (':'):
				_nbr_eol = 0;
				new_token.tkType = COLON;
				new_token.lexeme = ":";
				_token_list.push_back(new_token);
				++_it;
				break ;
			case (','):
				_nbr_eol = 0;
				new_token.tkType = COMA;
				new_token.lexeme = ",";
				_token_list.push_back(new_token);
				++_it;
				break ;
			case ('\r'):
				switch (peek()) {
					case ('\n'):
						new_token.tkType = EOL;
						new_token.lexeme = "\\r\\n";
						_token_list.push_back(new_token);
						_tokenizing = 4;
						_nbr_eol = 1;
						_it = _it + 2;
						break ;
					case ('\0'):
						break ;
					default: 
						new_token.tkType = ERROR;
						new_token.lexeme = "ERROR";
						_token_list.push_back(new_token);
						return (_token_list);
				}
				break ;
			default:
				_nbr_eol = 0;
				new_token.tkType = WORD;
				if (!_token_list.empty() && (_token_list.back().tkType == COLON || _token_list.back().tkType == COMA))
					new_token.lexeme = getWord(",\r");
				else
					new_token.lexeme = getWord(":,\r");
				if (!new_token.lexeme.empty())
					_token_list.push_back(new_token);
		}
		if (_it == _input.begin())
			break ;
		_input.erase(0, _it - _input.begin());
		_it = _input.begin();
	}
	return (_token_list);
}

std::string	HTTPTokenizer::getWord(std::string delim_list) {
	std::string::const_iterator	end_word = _it;
	while (end_word != _input.end()
		&& (*end_word == ' ' || *end_word == '\t')) {
		end_word++;
	}
	while (end_word != _input.end()
		&& delim_list.find(*end_word) == std::numeric_limits<unsigned long>::max()) {
		end_word++;
	}
	std::string	word("");
	if (end_word != _input.end()) {
		word = _input.substr(0, end_word - _input.begin());
		trimStart(word);
		_it = end_word;
	}
	return (word);
}

void	HTTPTokenizer::trimStart(std::string& string) {
	std::string::iterator	it = string.begin();
	while (it != string.end()
		&& (*it == ' ' || *it == '\t')) {
		++it;
	}
	if (it != string.begin())
		string.erase(0, it - string.begin());
	return ;
}

std::string	HTTPTokenizer::getInput() const {
	return (_input);
}

std::string	HTTPTokenizer::extractInput(size_t len) {
	std::string	tmp = _input.substr(0, len);
	if (!tmp.empty())
		_input.erase(0, len);
	return (tmp);
}

std::string	HTTPTokenizer::extractInput(char character) {
	size_t	pos	= _input.find(character);
	std::string	tmp;
	if (pos != std::string::npos) {
		tmp = _input.substr(0, pos);
		_input.erase(0, pos + 1);
	}
	return (tmp);
}

void	HTTPTokenizer::cleanTokenList() {
	_token_list.clear();
	_tokenizing = 0;
	_nbr_eol = 0;
}

bool	HTTPTokenizer::addInput(std::string input) {
	_input += input;
	_it = _input.begin();
	return (true);
}

void	HTTPTokenizer::advance() {
	if (_list_it != _token_list.end()) {
		_token_list.erase(_list_it);
		_list_it = _token_list.begin();
	}
	return ;
}

std::vector<t_Token>	HTTPTokenizer::getTokenList() const {
	return (_token_list);
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
		default:
	return ("\0");
	}
}
