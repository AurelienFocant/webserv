/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:00:18 by stempels          #+#    #+#             */
/*   Updated: 2025/12/09 14:56:57 by stempels         ###   ########.fr       */
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
	complete = false;
	status_code = INIT_STATE;
	content_length = -1;
	std::cout << "Default constructor called: Request" << std::endl;
}

Request::Request(std::string const& request) : HTTPTokenizer(request) {
	complete = false;
	status_code = INIT_STATE;
	content_length = -1;
	std::cout << "String constructor called: Request" << std::endl;
	if (!parseRequest()) {}
}

/*Public Methods*/
bool	Request::parseRequest() {
	std::vector<t_Token>	token_list = scanTokens();
	std::vector<t_Token>::const_iterator	it = token_list.begin();
	
	for (std::vector<t_Token>::const_iterator it = token_list.begin(); it != token_list.end(); it++) {
		std::cout << Tokenizer::getTokenType(*it) << '\t' << (*it)._lexeme << std::endl;
	}
	std::cout << "----------------------------------------" << std::endl;

	//Parsing for mandatory first line information
	if (!setMethod(it)
			|| !setRequestUrl(it)
			|| !setHttpVersion(it)
			|| (*it)._tkType != EOL) {
		if (status_code == INIT_STATE)
			status_code = BAD_REQUEST;
		complete = true;
		return (false);
	}
	it++;

	//Parsing header
	while (it != token_list.end() && it->_tkType != EOL) {
		while (it->_tkType == WORD) {
			std::string	options_name = normalizeOptions(it->_lexeme);
			it++;
			if (it->_tkType == COLON) {
				it++;
				if (it->_tkType == WORD) {
					while (it->_tkType == WORD) {
						detectImportantValue(options_name, it->_lexeme);
						options.insert(std::make_pair(options_name, it->_lexeme));		
						it++;
						if (it->_tkType != COMA)
							break ;
						it++;
					}
				}
				else {
					complete = true;
					status_code = BAD_REQUEST;
					return (false);
				}
			}
			else {
				complete = true;
				status_code = BAD_REQUEST;
				return (false);
			}
			if (it->_tkType == EOL)
				it++;
		}
		if (it->_tkType != EOL)
			return (false);
	}
	it++;

	//Check for GET request
	if (!method.compare("GET")) {
		complete = true;
		if (it == token_list.end())
			status_code = OK;
		else
			status_code = BAD_REQUEST;
	}

	//Parsing body if POST request
	if (!method.compare("POST")) {
		if (content_length == std::numeric_limits<unsigned long>::max()) {
			complete = true;
			status_code = LENGTH_REQUIRED;
		}
		else if (it->_tkType == WORD && it->_lexeme.size() == content_length) {
			complete = true;
			status_code = OK;
		}
		else
			return (false);
	}
	return (true);
}

std::string	Request::normalizeOptions(std::string argument) {
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
			content_length = std::atol(value.c_str()); 
			break ;
		default:
			break ; 
	}
	return ;
}

/*Getter*/
bool	Request::setMethod(std::vector<t_Token>::const_iterator& it) {
	t_Token	token = *it;
	if (token._tkType != WORD)	
		return (false);
	if (authorized_method.find(token._lexeme) != std::numeric_limits<unsigned long>::max()) {
		method = token._lexeme;
		it++;
		return (true);
	}
	if (unimplemented_method.find(token._lexeme) != std::numeric_limits<unsigned long>::max()) {
		method = "Error";
		status_code = NOT_IMPLEMENTED;
		return (false);
	}
	return (false);
}

bool	Request::setRequestUrl(std::vector<t_Token>::const_iterator& it) {
	t_Token	token = *it;
	if (token._tkType != WORD)
		return (false);
	if (token._lexeme[0] == '/') {
		request_url = token._lexeme;
		it++;
		return (true);
	}
	return (false);
}

bool	Request::setHttpVersion(std::vector<t_Token>::const_iterator& it) {
	t_Token	token = *it;
	if (token._tkType != WORD)
		return (false);
	if (!token._lexeme.compare("HTTP/1.0") || !token._lexeme.compare("HTTP/1.1")) {
		http_version = token._lexeme;
		it++;
		return (true);
	}
	return (false);
}

std::ostream&	operator<<(std::ostream& ostream, Request& other) {
	ostream << other.getMethod() << '\t' << other.getRequestUrl() << '\t' << other.getHttpVersion() << '\n';
	for (std::multimap<std::string, std::string>::const_iterator	it = other.getOptions().begin(); it != other.getOptions().end(); it++) {
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
