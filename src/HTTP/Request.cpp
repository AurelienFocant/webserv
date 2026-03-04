/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */

/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:18:42 by stempels          #+#    #+#             */
/*   Updated: 2026/02/25 13:40:43 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

const std::string	Request::authorized_method = "GET POST";
const std::string	Request::unimplemented_method =
						"CONNECT DELETE HEAD OPTIONS PATCH PUT TRACE";

/*Constructor - Copy Constructor - Destructor*/
Request::Request() : HTTPTokenizer()
{
	cleanRequest();
}

Request::Request(const Request& src)
	: HTTPTokenizer(src)
	, _progress(src._progress)
	, _complete(src._complete)
	, _status_code(src._status_code)
	, _body_handler(src._body_handler)	
	, _method(src._method)
	, _request_uri(src._request_uri)
	, _http_version(src._http_version)
	, _body(src._body)
	, _headers(src._headers)
	, _nbr_headers(src._nbr_headers)
	, _content_encoding(src._content_encoding)
	, _content_type(src._content_type)
	, _content_length(src._content_length) {
}

Request::Request(std::string const& request) : HTTPTokenizer(request)
{
	cleanRequest();
	parseRequest();
}

bool	Request::cleanRequest() {
	//Request State
	_progress = START;
	_complete = false;
	_status_code = INIT_STATE;

	//Request informations
	_method = NOT_SET;
	_request_uri.clear();
	_http_version.clear();
	_body.clear();

	//request usefull header informations
	_headers.clear();
	_nbr_headers = 0;
	_content_encoding = false;
	_content_type.clear();
	_content_length = std::numeric_limits<size_t>::max(); 

	//Tokenizer cleaning
	cleanTokenList();

	return (true);
}

/*Public Methods*/

// Parsing the request, progression status allow the parsing to stop and restart as the data arrive.
// _progress is set to the next case VALUE by each completed parsing operation --> see Httpenum.hpp for the list.
// _progress is set to PARSER_ERROR if an error in the request is detected.
// _progress is set to INTERNAL_SERVER_ERROR if the parser encounter a logic flow problem.

bool	Request::parseRequest() {
	// if more token are needed or available, create and add them to token list.
	if (_progress < PARSED)
		HTTPTokenizer::scanTokens();
	if (_token_list.empty())
		return (_complete);
	_list_it = _token_list.begin();
	
	// std::cout << "Request.cpp -l63:\n" << _token_list; // debug info, clean it before release.

	if (_progress < FIRST_LINE) {
		parseFirstLine();
	}
	if (_progress < PARSED) {
		parseHeader();
	}
	if (_progress == PARSED) {
		if (!isFirstLineValid()) {
			_progress = DONE;
			_complete = true;
			return (_complete);
		}

		if (!extractHeadersInformations()
			|| !areHeadersValid() 
			|| !areMandatoryHeadersPresent()) {
			_progress = DONE;
			_complete = true;
			if (_status_code == INIT_STATE)
				_status_code = BAD_REQUEST;
			return (_complete);
		}
		setupBodyHandler();
	}
	return (_complete);
}

bool	Request::handleBody(unsigned int max_body) {
		//add body size checking
		if (_progress == BODY_HANDLING)
			(this->*_body_handler)(max_body);

		// std::cout << "Request.cpp -l95: " << _progress << std::endl; // debug info, clean it before release.

		if (_complete)
			cleanTokenList();
		return (_complete);
}

/*Private Methods*/

void	Request::parseFirstLine() {
	//iterate and consume token list
	while (_progress < FIRST_LINE && _list_it != _token_list.end()) { 
		switch (_list_it->tkType) {
			case (WORD):
				extractFirstLineInfo();
				break ;
			case (EOL):
				if (_progress == VERSION) {
					_progress = FIRST_LINE;
					break ;
				}	// fall thru
			case (ERROR):
				_complete = true;
				_progress = DONE;
				_status_code = BAD_REQUEST;
				break ;
			default:
				_complete = true;
				_progress = DONE;
				_status_code = INTERNAL_SERVER_ERROR;
		}
		advance();
	}
	return ;
}

void	Request::parseHeader() {
	int	seq_pos = 0;
	t_tokenType	sequence[4] = {WORD, COLON, WORD, COMA};
	int	nbr_eol = 1; //first EOL at end of first line
	while (_progress < PARSED && _list_it != _token_list.end()) {
			if (_list_it->tkType == EOL) { 
				nbr_eol++;
				if (nbr_eol == 2)
					_progress = PARSED;
				seq_pos = 0;
			}
			else if (_list_it->tkType == sequence[seq_pos]) {
				nbr_eol = 0;
				if (_list_it->tkType == WORD)
					seq_pos++;
				else
					seq_pos = 2;
			}
			else {
				_progress = DONE;
				_complete = true;
				_status_code = BAD_REQUEST;
			}
			++_list_it;
	}
	return ;
}

void	Request::extractFirstLineInfo() {
	//Assign WORD token value to the correct attribute in Request
	//depending on the current progression
	switch (_progress) {
		case (START):
			_method = methodFromString((*_list_it).lexeme);
			_progress = METHOD;
			break ;
		case (METHOD):
			_request_uri = (*_list_it).lexeme;
			_progress = URI;
			break ;
		case (URI):
			_http_version = (*_list_it).lexeme;
			_progress = VERSION;
			break ;
		default:
			_complete = true;
			_progress = DONE;
			_status_code = BAD_REQUEST;
	}
	return ;
}

bool	Request::isFirstLineValid() {
	if (_method == NOT_SET) {
		_status_code = INTERNAL_SERVER_ERROR;
		return (false);
	}
	else if (_method == UNKNOWN) {
		_status_code = NOT_IMPLEMENTED;
		return (false);
	}
	if (!(_http_version == "HTTP/1.0" || _http_version == "HTTP/1.1")) {
		_status_code = BAD_REQUEST;
		return (false);
	}
	return (true);
}

bool	Request::setupBodyHandler() {
	if (_method == GET) { //Check for GET request
		_progress = DONE;
		_complete = true;
		_status_code = OK;
	}
	else if (_method == POST) { //Parsing body if POST request
		_progress = BODY_HANDLING;
		defineBodyExtractionHandler();
	}
	else if (_method == DELETE) { //Check for GET request
		_progress = DONE;
		_complete = true;
		_status_code = OK;
	}
	else {
		_complete = true;
		_status_code = BAD_REQUEST;
	}
	return (_complete);
}

bool	Request::defineBodyExtractionHandler() {
	if (_content_encoding) {
		_body_handler = &Request::bodyHandlerTransfertEncoding;
	}
	else if (_content_length != std::numeric_limits<unsigned long>::max()) {
		_body_handler = &Request::bodyHandlerContentLength; 
	}
	else {
		_progress = DONE;
//		_complete = true;
//		_status_code = NOT_IMPLEMENTED;
		return (false);
	}
	_progress = BODY_HANDLING;
	return (true);
}

bool	Request::bodyHandlerTransfertEncoding(unsigned int max_body) {
	if (_content_length == 0 
		|| _content_length == std::numeric_limits<unsigned long>::max()) {
		std::string	dft = extractInput('\n');
		if (dft.empty())
			return (_complete);
		dft.erase(dft.find('\r'));
		std::stringstream	ss;
		ss << std::hex << dft;
		ss >> _content_length; 
		if (_content_length)
			_content_length += 2;
		else {
			_progress = DONE;
			_complete = true;
			_status_code = OK;
			_content_length = std::atol(getHeaderValues("CONTENT_LENGTH").at(0).c_str());
		}
	}
	else {
		//check for max_body size
		size_t	before_len = _body.size();
		if (before_len + _content_length > max_body) {
			_progress = DONE;
			_complete = true;
			_status_code = REQUEST_ENTITY_TOO_LARGE;
		}
		else {
			_body += extractInput(_content_length);
			_content_length -= (_body.size() - before_len);
		}
	}
	return (_complete);
}

bool	Request::bodyHandlerContentLength(unsigned int max_body) {
	//check for max_body size
	if (max_body < _content_length) {
		_progress = DONE;
		_complete = true;
		_status_code = REQUEST_ENTITY_TOO_LARGE;
	}
	else {
		size_t	before_len = _body.size();
		_body += extractInput(_content_length);
		_content_length -= (_body.size() - before_len);
		if (_content_length == 0) {
			_progress = DONE;
			_complete = true;
			_status_code = OK;
			_content_length = std::atol(getHeaderValues("CONTENT_LENGTH").at(0).c_str());
		}
	}
	return (_complete);
}

bool	Request::bodyHandlerMultipart() {
	static std::string	ender;
	if (ender.empty()) {
		std::string	delimiter = _content_type.substr(_content_type.find('=') + 1);
		delimiter.erase(delimiter.find('"'), 1);
		delimiter.erase(delimiter.find('"'), 1); //CHANGE that after
		std::string	ender = "--" + delimiter + "--";
	}
	size_t	len = 1000;
	_body += extractInput(len);//FIND another way after proof of concept
	if (_body.find(ender) != std::numeric_limits<unsigned long>::max()) {
		ender.clear();
		_complete = true;
		_status_code = OK;
	}
	return (_complete);
}

bool	Request::extractHeadersInformations() {
	std::vector<t_Token>::const_iterator	it = _token_list.begin();
	std::string								options_name;
	while (it != _token_list.end()) {
		switch (it->tkType) {
			case (WORD):
				options_name = normalizeHeadersKey(it->lexeme);
				break ;
			case (EOL):
				options_name.clear();
				break ;
			default: //COLON, COMA
				it++;
				if (!detectImportantValue(options_name, it->lexeme))
					return (false);
				safeInsertion(options_name, it->lexeme);
		}
		it++;
	}
	return (true);
}

void	Request::safeInsertion(const std::string& key, const std::string& value) {
		std::pair  <std::multimap<std::string, std::string>::iterator,
					std::multimap<std::string, std::string>::iterator>	range = _headers.equal_range(key);
		std::multimap<std::string, std::string>::iterator	it = range.first;
		while (it != range.second) {
			if (it->second == value)
				break ;
			it++;
		}
		if (it == range.second)
				_headers.insert(std::make_pair(key, value));		
		return ;
}

bool	Request::areHeadersValid() const {
	std::multimap<std::string, std::string>::const_iterator it = _headers.begin();
	if (_http_version == "HTTP/1.0") {
		const char*		uniqueHeadersHttp_0[2] = {
			"CONTENT_LENGTH", NULL
			};
			while (it != _headers.end()) {
				int nbr_headers = _headers.count(it->first);
				if (isUniqueHeader(it->first, uniqueHeadersHttp_0) &&  nbr_headers > 1)
					return (false);
				while (it != _headers.end() && nbr_headers > 0)
					++it;
			}
	}
	else if (_http_version == "HTTP/1.1") {
		const char*		uniqueHeadersHttp_1[2] = { 
			"CONTENT_LENGTH", NULL
			};
			while (it != _headers.end()) {
				int nbr_headers = _headers.count(it->first);
				if (isUniqueHeader(it->first, uniqueHeadersHttp_1) &&  nbr_headers > 1)
					return (false);
				while (it != _headers.end() && nbr_headers > 0)
					++it;
			}
	}
	if (_content_length != std::numeric_limits<size_t>::max() && _content_encoding == true)
		return (false);
	return (true);
}

bool	Request::isUniqueHeader(const std::string& header_key, const char** unique_list) const {
	int i = 0;
	while (unique_list[i])
		i++;
	i -= 1;
	for (; i >= 0; --i) { 
		if (header_key == unique_list[i])
			return (true);
	}
	return (false);
}

bool	Request::areMandatoryHeadersPresent() const {
	if (_http_version == "HTTP/1.0") {
		const char*		mandatoryHeadersHttp_0[1] = {
			NULL
		};
		int i = 0;
		while (mandatoryHeadersHttp_0[i]) {
			if (_headers.count(mandatoryHeadersHttp_0[i]) < 1)
				return (false);
			++i;
		}
	}
	else if (_http_version == "HTTP/1.1") {
		const char*		mandatoryHeadersHttp_1[2] = {
		"HOST", NULL
		};
		int i = 0;
		while (mandatoryHeadersHttp_1[i]) {
			if (_headers.count(mandatoryHeadersHttp_1[i]) < 1)
				return (false);
			++i;
		}
	}
	return (true);
}

std::string	Request::normalizeHeadersKey(std::string argument) const {
	for (std::string::iterator	it = argument.begin(); it != argument.end(); it++) {
		if (*it == '-')
			*it = '_';
		else
			*it = std::toupper(*it);
	}
	return (argument);
}

bool	Request::detectImportantValue(std::string& argument, std::string value) {
	const char*	important_argument[3] = {
		"CONTENT_LENGTH", "TRANSFERT_ENCODING", NULL
		};

	int	i = 0;
	while (important_argument[i] && important_argument[i] != argument)
		i++;
	switch (i) {
		case (sizeof(important_argument)):
			break ;
		case (0):
			_content_length = std::atol(value.c_str()); 
			break ;
		case (1):
			if (value.empty()) {
				_status_code = BAD_REQUEST;
				return (false);
			}
			else if (value != "chunked") {
				_status_code = NOT_IMPLEMENTED; //FIND CORRECT ERROR
				break ;
			}
			else {
				_content_encoding = true;
				return (false);
			}
		default:
			break ; 
	}
	return (true);
}

/*Getter*/
t_method		Request::getMethod() const {
	return(_method);
}

std::string		Request::getRequestUri() const {
	return(_request_uri);
}

std::string		Request::getHttpVersion() const {
	return(_http_version);
}

std::string	Request::Request::getBody() const {
	return (_body);
}

bool			Request::isCompleted() const {
	return(_complete);
}

t_HttpCode		Request::getStatusCode() const {
	return(_status_code);
}

const int&				Request::getState() const {
	return (_progress);
}

size_t				Request::getContentLength() const {
	return (_content_length);
}

std::vector<std::string>	Request::getHeaderValues(std::string header_name) const {
	header_name = normalizeHeadersKey(header_name);
	std::pair<
		std::multimap<std::string, std::string>::const_iterator
	, std::multimap<std::string, std::string>::const_iterator>	range;
	range = _headers.equal_range(header_name);
	size_t	nbr_values = _headers.count(header_name);
	std::vector<std::string> values;
	values.reserve(nbr_values);
	std::multimap<std::string, std::string>::const_iterator it = range.first;
	for (; nbr_values > 0; --nbr_values) {
		values.push_back(it->second);
		++it;
	}
	if (values.empty())
		values.insert(values.begin(), "");
	return (values);
}

const std::multimap<std::string, std::string>&	Request::getHeaders() const {
	return (_headers);
}

/*Setters*/
bool	Request::addInput(const std::string& input) {
	HTTPTokenizer::addInput(input);
	return (true);
}

std::ostream&	operator<<(std::ostream& ostream, Request& other) {
	ostream << other.getMethod() << '\t' << other.getRequestUri() << '\t' << other.getHttpVersion() << '\n';
	for (std::multimap<std::string, std::string>::const_iterator	it = other.getHeaders().begin(); it != other.getHeaders().end(); it++) {
		std::cout << it->first << ' ' << it->second << '\n';
	}
	if (!other.getBody().empty())
		ostream << other.getBody() << std::endl;
	std::cout << other.getStatusCode() << ' ' << httpStatusToString(other.getStatusCode()) << std::endl;
	ostream << "----------------------------------------\n" << std::endl;
	return (ostream);
}

std::ostream&	operator<<(std::ostream& ostream, std::vector<t_Token>& token_list) {
	ostream << "\n----------------------------------------" << std::endl;
	for (std::vector<t_Token>::const_iterator it = token_list.begin(); it != token_list.end(); it++) {
		ostream << HTTPTokenizer::getTokenType(*it) << '\t' << (*it).lexeme << std::endl;
	}
	ostream << "----------------------------------------" << std::endl;
	return (ostream);
}
