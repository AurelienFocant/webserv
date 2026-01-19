#include "Response.hpp"
	
Response::Response()
: _state(SEND_HEADER)
, _body_type(STATIC)
, _status_code(OK)
, _http_version("")
, _header_sent(0)
, _fd(-1)
, _body_size(0)
, _body_sent (0)
, _buffer_size(0)
, _buffer_sent(0)
{
	std::memset(_buffer, 0, BUFFER_SIZE);
}

Response::~Response()
{
	if (_fd != -1)
		close(_fd);
	std::cout << "Destructor called: Response" << std::endl;
}

void	Response::formatResponse()
{
	std::string	formatted = buildHttpResponse();

	if (formatted.size() > BUFFER_SIZE)
	{
		//Doc NGINX: A request header field cannot exceed the size of one buffer as well
		//, or the 400 (Bad Request) error is returned to the client. Buffers are allocated only on demand.
		//By default, the buffer size is equal to 8K bytes.

		_status_code = INTERNAL_SERVER_ERROR;

		std::cerr << "[Error] Response header size exceed buffer size " << formatted.size() << " bytes" << std::endl;
		formatted = "HTTP/1.1 500 Internal Server Error\r\n"
					"Content-Lenght: 0\r\n"  //Content-lenght = error file size
					"Connection: close\r\n"
					"\r\n";
		return;
	}

	std::memcpy(_buffer, formatted.c_str(), formatted.size());

	_buffer_size = formatted.size();
	_buffer_sent = 0;
	_state = SEND_HEADER;

}


std::string	Response::buildHttpResponse()
{
	std::stringstream response;


	response << _http_version << "" << _status_code << ""
			 << httpStatusToString(static_cast<t_HttpCode>(_status_code)) << "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
	it != _headers.end(); it++)
		response << it->first << ": " << it->second << "\r\n";
	response << "\r\n";

	return response.str();
}

/* Getters / Setters */

int	Response::getState() const
{
	return _state;
}

void	Response::setState(int state)
{
	_state = state;
}

int	Response::getStatusCode() const
{
	return _status_code; 
}

void	Response::setStatusCode(int status_code)
{
	_status_code = status_code;
}

void Response::setHttpVersion(const std::string& version)
{
	_http_version = version;
}

std::string	Response::getHttpVersion() const
{
	return _http_version;
}


std::string	Response::getHeader(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return it->second;
	return ""; 
}

void	Response::setHeader(const std::string& key, const std::string& value)
{
	_headers[key] = value;
}


int	Response::getBodyFd() const
{
	return _fd;
}

void	Response::setBodyFd(int fd)
{
	_fd = fd;
}

int	Response::getBodySize() const
{
	return _body_size;
}

void	Response::setBodySize(int size)
{
	_body_size = size;
}


/* std::string	Response::getBodyContent() const
{
	return _content;
} */

void	Response::setBodyContent(const std::string& content)
{
	_body_content = content;
}

void	Response::resetBuffer()
{
	_buffer_size = 0;
	_buffer_sent = 0;
}

void	Response::cleanResponse()
{

	_state = SEND_HEADER;
	_body_type = STATIC;
	_status_code = OK;
	_http_version = "";

	_header_sent = 0;
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
	_body_size = 0;
	_body_sent = 0;
	_buffer_size = 0;
	_buffer_sent = 0;
	std::memset(_buffer, 0, BUFFER_SIZE);
}


std::ostream& operator<<(std::ostream& os, const Response& response)
{
	os << "[Response] Status: " << response._status_code 
		<< ", Type: " << response._body_type
	   << ", Body: " << response._body_size << " bytes"
	   << ", FD: " << response._fd;
	return os;
}
