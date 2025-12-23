/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:00:18 by stempels          #+#    #+#             */
/*   Updated: 2025/12/23 15:35:27 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

const std::string	Request::authorized_method = "GET POST";
const std::string	Request::unimplemented_method =
						"CONNECT DELETE HEAD OPTIONS PATCH PUT TRACE";
const char*			Request::important_argument[] = {
	"content-length", "content-type", "transfert-encoding"
	};

/*Constructor - Copy Constructor - Destructor*/
Request::Request() : HTTPTokenizer() {
	cleanRequest();
	std::cout << "Default constructor called: Request" << std::endl;
}

Request::Request(std::string const& request) : HTTPTokenizer(request) {
	cleanRequest();
	std::cout << "String constructor called: Request" << std::endl;
	if (!parseRequest()) {}
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
bool	Request::parseRequest() {
	if (_progress < PARSED)
		HTTPTokenizer::scanTokens();
	_list_it = _token_list.begin();
	
	std::cout << "Request.cpp -l63:\n" << _token_list;

	//Parsing for mandatory first line information
	if	(!parseFirstLine()) {
		_complete = true;
		_status_code = BAD_REQUEST;
	}

	//Parsing headers information
	switch (_progress) {
		case (FIRST_LINE):
			if (!parseHeader())
				break ;
			//else fall_through;
/*		case (PARSED):
			if (parseHeader())
				break ;
			//else fall_through; 
*/
		case (PARSED):
			if (_status_code == INIT_STATE)
				handleBody();
			break ;
		case (BODY_HANDLING):
			if (_status_code == INIT_STATE) {
				if ((this->*_body_handler)()) {
					// SOMETHING
				}
			}
			break ;
		case (PARSER_ERROR):
			_progress = PARSER_ERROR;
			_complete = true;
			_status_code = BAD_REQUEST;
			break ;
		default:
			_progress = PARSER_ERROR;
			_complete = true;
			_status_code = INTERNAL_SERVER_ERROR;
	}
	removeEOC();

	std::cout << "Request.cpp -l95: " << _progress << std::endl;

	return (_complete);
}

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
				if ((*_list_it)._tkType == EOL) {
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

bool	Request::handleBody() {
	if (_method == GET) { //Check for GET request
		if (_progress == PARSED) {
			extractHeadersInformations();
			_progress = DONE;
			_complete = true;
			_status_code = OK;
		}
	}
	else if (_method == POST) { //Parsing body if POST request
		extractHeadersInformations();
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
	else if (_content_type == "multipart") {
		_body_handler = &Request::bodyHandlerMultipart; 
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
	if (_list_it == _token_list.end() || _list_it->_tkType == EOC)
		return (true);
	int	nbr_eol = 0;
	while (_list_it->_tkType != EOC && nbr_eol != 2) {
		switch (_list_it->_tkType) {
			case (WORD):
				nbr_eol = 0;
				if ((++_list_it)->_tkType == COLON && (++_list_it)->_tkType == WORD) {
					while (_list_it->_tkType == WORD) {
						_list_it++;
						_nbr_headers++;
						if (_list_it->_tkType == COMA)
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
//			case (ERROR):
				//fall-through
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
		switch (it->_tkType) {
			case (WORD):
				options_name = normalizeHeadersKey(it->_lexeme);
				break ;
			case (EOL):
				options_name.clear();
				break ;
			default: //COLON, COMA
				it++;
				detectImportantValue(options_name, it->_lexeme);
				safeInsertion(options_name, it->_lexeme);
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

std::string	Request::normalizeHeadersKey(std::string argument) {
	for (std::string::iterator	it = argument.begin(); it != argument.end(); it++) {
		*it = std::tolower(*it);
	}
	return (argument);
}

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
			_content_type = value;
			break ;
		case (2):
			_content_encoding = true;
			break ;
		default:
			break ; 
	}
	return ;
}

/*Getter*/
std::string	Request::getBody() const {
	return (_body);
}
	
bool	Request::setMethod() {
	if (_progress == PARSER_ERROR)
		return (false);
	t_Token	token = *_list_it;
	if (token._tkType != WORD) {
		_progress = PARSER_ERROR;
		return (false);
	}
	_method = idMethod(token._lexeme);
	_list_it++;
	_progress = METHOD;
	return (true);
}

bool	Request::setRequestUri() {
	if (_progress == PARSER_ERROR)
		return (false);
	t_Token	token = *_list_it;
	switch (token._tkType) {
		case (EOC):
			break ;
		case (WORD):
			_request_uri = token._lexeme;
			_list_it++;
			_progress = URI;
			break ;
		default:
			_progress = PARSER_ERROR;
			return (false);
	}
	return (true);
}

bool	Request::setHttpVersion() {
	if (_progress == PARSER_ERROR)
		return (false);
	t_Token	token = *_list_it;
	switch (token._tkType) {
		case (EOC):
			break ;
		case (WORD):
			_http_version = token._lexeme;
			_list_it++;
			_progress = VERSION;
			break ;
		default:
			_progress = PARSER_ERROR;
			return (false);
	}
	return (true);
}

t_Method	Request::idMethod(std::string& method) {
	if (method.find("GET") == 0)
		return (GET);
	else if (method.find("POST") == 0)
		return (POST);
	else
		return (UNKNOWN);
}

bool	Request::addInput(std::string input) {
	HTTPTokenizer::addInput(input);
	return (true);
}

std::ostream&	operator<<(std::ostream& ostream, Request& other) {
	ostream << other.getMethod() << '\t' << other.getRequestUri() << '\t' << other.getHttpVersion() << '\n';
	for (std::multimap<std::string, std::string>::const_iterator	it = other.getHeadersValue().begin(); it != other.getHeadersValue().end(); it++) {
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
		ostream << HTTPTokenizer::getTokenType(*it) << '\t' << (*it)._lexeme << std::endl;
	}
	ostream << "----------------------------------------" << std::endl;
	return (ostream);
}

std::string httpStatusToString(t_HttpCode code)  {
    switch (code)  {
		case INIT_STATE:					  return "Initialized";
        // --- 1xx ---
        case CONTINUE:                        return "Continue";
        case SWITCHING_PROTOCOLS:             return "Switching Protocols";

        // --- 2xx ---
        case OK:                              return "OK";
        case CREATED:                         return "Created";
        case ACCEPTED:                        return "Accepted";
        case NON_AUTHORITATIVE_INFORMATION:   return "Non-Authoritative Information";
        case NO_CONTENT:                      return "No Content";
        case RESET_CONTENT:                   return "Reset Content";
        case PARTIAL_CONTENT:                 return "Partial Content";

        // --- 3xx ---
        case MULTIPLE_CHOICES:                return "Multiple Choices";
        case MOVED_PERMANENTLY:               return "Moved Permanently";
        case FOUND:                           return "Found";
        case SEE_OTHER:                       return "See Other";
        case NOT_MODIFIED:                    return "Not Modified";
        case USE_PROXY:                       return "Use Proxy";
        case TEMPORARY_REDIRECT:              return "Temporary Redirect";

        // --- 4xx ---
        case BAD_REQUEST:                     return "Bad Request";
        case UNAUTHORIZED:                    return "Unauthorized";
        case PAYMENT_REQUIRED:                return "Payment Required";
        case FORBIDDEN:                       return "Forbidden";
        case NOT_FOUND:                       return "Not Found";
        case METHOD_NOT_ALLOWED:              return "Method Not Allowed";
        case NOT_ACCEPTABLE:                  return "Not Acceptable";
        case PROXY_AUTHENTICATION_REQUIRED:   return "Proxy Authentication Required";
        case REQUEST_TIMEOUT:                 return "Request Timeout";
        case CONFLICT:                        return "Conflict";
        case GONE:                            return "Gone";
        case LENGTH_REQUIRED:                 return "Length Required";
        case PRECONDITION_FAILED:             return "Precondition Failed";
        case REQUEST_ENTITY_TOO_LARGE:        return "Request Entity Too Large";
        case REQUEST_URI_TOO_LONG:            return "Request-URI Too Long";
        case UNSUPPORTED_MEDIA_TYPE:          return "Unsupported Media Type";
        case REQUESTED_RANGE_NOT_SATISFIABLE: return "Requested Range Not Satisfiable";
        case EXPECTATION_FAILED:              return "Expectation Failed";

        // --- 5xx ---
        case INTERNAL_SERVER_ERROR:           return "Internal Server Error";
        case NOT_IMPLEMENTED:                 return "Not Implemented";
        case BAD_GATEWAY:                     return "Bad Gateway";
        case SERVICE_UNAVAILABLE:             return "Service Unavailable";
        case GATEWAY_TIMEOUT:                 return "Gateway Timeout";
        case HTTP_VERSION_NOT_SUPPORTED:      return "HTTP Version Not Supported";
    }

    return "Unknown Status";
}

t_HttpCode httpStatusFromString(const std::string& s)  {
    // --- 1xx ---
    if (s == "Continue")                        return CONTINUE;
    if (s == "Switching Protocols")             return SWITCHING_PROTOCOLS;

    // --- 2xx ---
    if (s == "OK")                              return OK;
    if (s == "Created")                         return CREATED;
    if (s == "Accepted")                        return ACCEPTED;
    if (s == "Non-Authoritative Information")   return NON_AUTHORITATIVE_INFORMATION;
    if (s == "No Content")                      return NO_CONTENT;
    if (s == "Reset Content")                   return RESET_CONTENT;
    if (s == "Partial Content")                 return PARTIAL_CONTENT;

    // --- 3xx ---
    if (s == "Multiple Choices")                return MULTIPLE_CHOICES;
    if (s == "Moved Permanently")               return MOVED_PERMANENTLY;
    if (s == "Found")                           return FOUND;
    if (s == "See Other")                       return SEE_OTHER;
    if (s == "Not Modified")                    return NOT_MODIFIED;
    if (s == "Use Proxy")                       return USE_PROXY;
    if (s == "Temporary Redirect")              return TEMPORARY_REDIRECT;

    // --- 4xx ---
    if (s == "Bad Request")                     return BAD_REQUEST;
    if (s == "Unauthorized")                    return UNAUTHORIZED;
    if (s == "Payment Required")                return PAYMENT_REQUIRED;
    if (s == "Forbidden")                       return FORBIDDEN;
    if (s == "Not Found")                       return NOT_FOUND;
    if (s == "Method Not Allowed")              return METHOD_NOT_ALLOWED;
    if (s == "Not Acceptable")                  return NOT_ACCEPTABLE;
    if (s == "Proxy Authentication Required")   return PROXY_AUTHENTICATION_REQUIRED;
    if (s == "Request Timeout")                 return REQUEST_TIMEOUT;
    if (s == "Conflict")                        return CONFLICT;
    if (s == "Gone")                            return GONE;
    if (s == "Length Required")                 return LENGTH_REQUIRED;
    if (s == "Precondition Failed")             return PRECONDITION_FAILED;
    if (s == "Request Entity Too Large")        return REQUEST_ENTITY_TOO_LARGE;
    if (s == "Request-URI Too Long")            return REQUEST_URI_TOO_LONG;
    if (s == "Unsupported Media Type")          return UNSUPPORTED_MEDIA_TYPE;
    if (s == "Requested Range Not Satisfiable") return REQUESTED_RANGE_NOT_SATISFIABLE;
    if (s == "Expectation Failed")              return EXPECTATION_FAILED;

    // --- 5xx ---
    if (s == "Internal Server Error")           return INTERNAL_SERVER_ERROR;
    if (s == "Not Implemented")                 return NOT_IMPLEMENTED;
    if (s == "Bad Gateway")                     return BAD_GATEWAY;
    if (s == "Service Unavailable")             return SERVICE_UNAVAILABLE;
    if (s == "Gateway Timeout")                 return GATEWAY_TIMEOUT;
    if (s == "HTTP Version Not Supported")      return HTTP_VERSION_NOT_SUPPORTED;

    // Default fallback
    return INTERNAL_SERVER_ERROR;
}
