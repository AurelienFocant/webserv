/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:00:18 by stempels          #+#    #+#             */
/*   Updated: 2025/12/11 19:04:52 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

const std::string	Request::authorized_method = "GET POST";
const std::string	Request::unimplemented_method =
						"CONNECT DELETE HEAD OPTIONS PATCH PUT TRACE";
const char*			Request::important_argument[] = {
	"content_length"
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
	_complete = false;
	_status_code = INIT_STATE;

	//Request informations
	_method.clear();
	_request_uri.clear();
	_http_version.clear();
	_body.clear();

	//request usefull header informations
	_headers.clear();
	_content_length = -1; 

	return (true);
}

/*Public Methods*/
bool	Request::parseRequest() {
	HTTPTokenizer::scanTokens();
	std::vector<t_Token>	token_list = getTokenList();
	std::vector<t_Token>::const_iterator	it = token_list.begin();
	
	for (std::vector<t_Token>::const_iterator it = token_list.begin(); it != token_list.end(); it++) {
		std::cout << HTTPTokenizer::getTokenType(*it) << '\t' << (*it)._lexeme << std::endl;
	}
	std::cout << "----------------------------------------" << std::endl;

	//Parsing for mandatory first line information
	if (!setMethod(it)
			|| !setRequestUrl(it)
			|| !setHttpVersion(it)
			|| (*it)._tkType != EOL
			) {
		if (_status_code == INIT_STATE)
			_status_code = BAD_REQUEST;
		return (_complete);
	}
	it++;
	if (it != token_list.end() && !parseHeader(it, token_list))
		return (_complete);

	//Check for GET request
	if (!_method.compare("GET")) {
		if (it == token_list.end())
			_status_code = OK;
		else
			_status_code = BAD_REQUEST;
	}

	//Parsing body if POST request
	if (!_method.compare("POST")) {
		if (_content_length == std::numeric_limits<unsigned long>::max()) {
			_complete = true;
			_status_code = LENGTH_REQUIRED;
		}
		else if (it->_tkType == WORD && it->_lexeme.size() == _content_length) {
			_complete = true;
			_status_code = OK;
			_body = it->_lexeme;
		}
		else
			return (_complete);
	}
	return (_complete);
}
	//Parsing header
bool	Request::parseHeader(std::vector<t_Token>::const_iterator& it, std::vector<t_Token>& token_list) {
	while (it != token_list.end() && it->_tkType != EOL) {
		while (it->_tkType == WORD) {
			std::string	options_name = normalizeHeadersKey(it->_lexeme);
			it++;
			if (it->_tkType == COLON) {
				it++;
				if (it->_tkType == WORD) {
					while (it->_tkType == WORD) {
						detectImportantValue(options_name, it->_lexeme);
						_headers.insert(std::make_pair(options_name, it->_lexeme));		
						it++;
						if (it->_tkType != COMA)
							break ;
						it++;
					}
				}
				else {
					_complete = true;
					_status_code = BAD_REQUEST;
					return (false);
				}
			}
			else {
				_complete = true;
				_status_code = BAD_REQUEST;
				return (false);
			}
			if (it->_tkType == EOL)
				it++;
		}
		if (it->_tkType != EOL)
			return (false);
	}
	it++;
	return (true);
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
		default:
			break ; 
	}
	return ;
}

/*Getter*/
bool	Request::setMethod(std::vector<t_Token>::const_iterator& it) {
	t_Token	token = *it;
	if (token._tkType != WORD) {
		_complete = true;
		return (false);
	}
	if (authorized_method.find(token._lexeme) != std::numeric_limits<unsigned long>::max()) {
		_method = token._lexeme;
		it++;
		return (true);
	}
	if (unimplemented_method.find(token._lexeme) != std::numeric_limits<unsigned long>::max()) {
		_method = "Error";
		_status_code = NOT_IMPLEMENTED;
		_complete = true;
		return (false);
	}
	return (false);
}

bool	Request::setRequestUrl(std::vector<t_Token>::const_iterator& it) {
	t_Token	token = *it;
	if (token._tkType != WORD) {
		_complete = true;
		return (false);
	}
	if (token._lexeme[0] == '/') {
		_request_uri = token._lexeme;
		it++;
		return (true);
	}
	return (false);
}

bool	Request::setHttpVersion(std::vector<t_Token>::const_iterator& it) {
	t_Token	token = *it;
	if (token._tkType != WORD) {
		_complete = true;
		return (false);
	}
	if (!token._lexeme.compare("HTTP/1.0") || !token._lexeme.compare("HTTP/1.1")) {
		_http_version = token._lexeme;
		it++;
		return (true);
	}
	return (false);
}

bool	Request::setInput(std::string input) {
	cleanRequest();
	HTTPTokenizer::setInput(input);
	return (true);
}

std::ostream&	operator<<(std::ostream& ostream, Request& other) {
	ostream << other.getMethod() << '\t' << other.getRequestUrl() << '\t' << other.getHttpVersion() << '\n';
	for (std::multimap<std::string, std::string>::const_iterator	it = other.getHeadersValue().begin(); it != other.getHeadersValue().end(); it++) {
		std::cout << it->first << ' ' << it->second << '\n';
	}
	std::cout << '\n' << other.getStatusCode() << ' ' << httpStatusToString(other.getStatusCode()) << std::endl;
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
