#include "Request.hpp"

const std::string	Request::authorized_method = "GET POST";
const std::string	Request::unimplemented_method =
						"CONNECT DELETE HEAD OPTIONS PATCH PUT TRACE";

/*Constructor - Copy Constructor - Destructor*/
Request::Request() : HTTPTokenizer()
{
	cleanRequest();
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
	_content_length = -1; 

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
	_list_it = _token_list.begin();
	
	std::cout << "Request.cpp -l63:\n" << _token_list; // debug info, clean it before release.

	//iterate and consume token list
	while (_progress < PARSED && (_list_it != _token_list.end() && _list_it->tkType != EOC)) { 
		switch (_progress) {
			case (START):
				setMethod();
				break ;
			case (METHOD):
				setRequestUri();
				break ;
			case (URI):
				if (setHttpVersion())
				break ;
			case (VERSION):
				if ((*_list_it).tkType == EOL)
					_progress = FIRST_LINE;
				break ;
			case (FIRST_LINE):
				parseHeader();
				break ;
			case (PARSER_ERROR):
				_complete = true;
				_progress = DONE;
				_status_code = BAD_REQUEST;
				break ;
			default:
				_complete = true;
				_progress = DONE;
				_status_code = INTERNAL_SERVER_ERROR;
		}
	}

	if (_progress == PARSED)
		handleBody();

	switch (_progress) {
		case (DONE):
			break ;
		case (BODY_HANDLING):
			(this->*_body_handler)();
			break ;
		case (PARSER_ERROR):
			_complete = true;
			_progress = DONE;
			_status_code = BAD_REQUEST;
			break ;
		default:
			_complete = true;
			_progress = DONE;
			_status_code = INTERNAL_SERVER_ERROR;
	}

	removeEOC();

	std::cout << "Request.cpp -l95: " << _progress << std::endl; // debug info, clean it before release.

	if (_complete)
		cleanTokenList();
	return (_complete);
}

/*
bool	Request::parseFirstLine() {
	while (_progress != PARSER_ERROR && _progress < FIRST_LINE && _list_it != _token_list.end()) {
		switch (_progress) {
			case (START):
				if (!setMethod())
					break ;
				//else fall-through
			case (METHOD):
				if (!setRequestUri())
					break ;
				//else fall-through
			case (URI):
				if (!setHttpVersion())
					break ;
				//else fall-through
			case (VERSION):
				if ((*_list_it).tkType == EOL) {
					_progress = FIRST_LINE;
					break ;
				}
				//else fall-through
			default:
				_progress = PARSER_ERROR;
		}
	}
	if (_progress == PARSER_ERROR)
		return (false);
	else
		return (true);
}
*/

bool	Request::handleBody() {
	extractHeadersInformations();
	/*if (!areHeadersValid()) {
		_progress = DONE;
		_complete = true;
		_status_code = BAD_REQUEST; //see if it is the correct status code
		return (_complete);
	}*/
	if (_method == GET) { //Check for GET request
		_progress = DONE;
		_complete = true;
		_status_code = OK;
	}
	else if (_method == POST) { //Parsing body if POST request
		_progress = BODY_HANDLING;
		defineBodyExtractionHandler();
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
		_complete = true;
		_status_code = NOT_IMPLEMENTED;
		return (false);
	}
	_progress = BODY_HANDLING;
	return (true);
}

bool	Request::bodyHandlerTransfertEncoding() {
	if (_content_length == 0 
		|| _content_length == std::numeric_limits<unsigned long>::max()) {
		std::string	dft = extractInput('\n');
		dft.erase(dft.find('\r'));
		std::stringstream	ss;
		ss << std::hex << dft;
		ss >> _content_length; 
		if (_content_length)
			_content_length += 2;
	}
	if (_content_length == 0) {
		_progress = DONE;
		_complete = true;
		_status_code = OK;
	}
	else {
		size_t	before_len = _body.size();
		_body += extractInput(_content_length);
		_content_length -= (_body.size() - before_len);
	}
	return (_complete);
}

bool	Request::bodyHandlerContentLength() {
	size_t	before_len = _body.size();
	_body += extractInput(_content_length);
	_content_length -= (_body.size() - before_len);
	if (_content_length == 0) {
		_progress = DONE;
		_complete = true;
		_status_code = OK;
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

bool	Request::parseHeader() {
	if (_list_it == _token_list.end() || _list_it->tkType == EOC)
		return (true);
	int	nbr_eol = 0;
	while (_progress != PARSER_ERROR && nbr_eol != 2 && _list_it->tkType != EOC) {
		switch (_list_it->tkType) {
			case (WORD):
				nbr_eol = 0;
				if ((++_list_it)->tkType == COLON && (++_list_it)->tkType == WORD) {
					while (_list_it->tkType == WORD) {
						_list_it++;
						_nbr_headers++;
						if (_list_it->tkType == COMA)
							_list_it++;
						else
							break ;
					}
				}
				break ;
			case (EOL):
				nbr_eol++;
				_list_it++;
				break ;
			case (EOC):
				break ;
			default:
				_progress = PARSER_ERROR;
				_complete = true;
				_status_code = BAD_REQUEST;
		}
	}
	if (nbr_eol == 2)
		_progress = PARSED;
	removeEOC();
	if (_progress != PARSED)
		return (false);
	return (true);
}

bool	Request::extractHeadersInformations() {
	std::vector<t_Token>::const_iterator	it = _token_list.begin();
	std::string								options_name;
//	_headers.reserve(_nbr_headers);
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
				detectImportantValue(options_name, it->lexeme);
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

std::string	Request::normalizeHeadersKey(std::string argument) const {
	for (std::string::iterator	it = argument.begin(); it != argument.end(); it++) {
		if (*it == '-')
			*it = '_';
		else
			*it = std::toupper(*it);
	}
	return (argument);
}

const char*			Request::important_argument[] = {
	"CONTENT_LENGTH", "TRANSFERT_ENCODING"
	};

void	Request::detectImportantValue(std::string& argument, std::string value) {
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
			if (value != "chunked")
				_status_code = BAD_REQUEST; //FIND CORRECT ERROR
			else
				_content_encoding = true;
			break ;
		default:
			break ; 
	}
	return ;
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
	return (values);
}

const std::multimap<std::string, std::string>&	Request::getHeaders() const {
	return (_headers);
}

/*Setters*/
bool	Request::setMethod() {
	if ((*_list_it).tkType == WORD) {
		_method = methodFromString((*_list_it).lexeme);
		_list_it++;
		_progress = METHOD;
		return (true);
	}
	_progress = PARSER_ERROR;
	return (false);
}

bool	Request::setRequestUri() {
	if ((*_list_it).tkType == WORD) {
		_request_uri = (*_list_it).lexeme;
		_list_it++;
		_progress = URI;
		return (true);
	}
	_progress = PARSER_ERROR;
	return (false);
}

bool	Request::setHttpVersion() {
	if ((*_list_it).tkType == WORD) {
			_http_version = (*_list_it).lexeme;
			_list_it++;
			_progress = VERSION;
		return (true);
	}
	_progress = PARSER_ERROR;
	return (false);
}

/*
   t_method	Request::idMethod(std::string& method) {
   if (method.find("GET") == 0)
   return (GET);
   else if (method.find("POST") == 0)
   return (POST);
   else
   return (UNKNOWN);
   }
 */

bool	Request::addInput(std::string input) {
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
