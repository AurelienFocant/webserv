#include "Response.hpp"
	
Response::Response()
: _state(SEND_HEADER)
, _body_type(STATIC)
, _status_code(OK)
, _http_version("")
, _header_sent(0)
, _body_fd(-1)
, _body_content("")
, _body_size(0)
, _body_sent(0)
, _buffer_size(0)
, _buffer_sent(0)
{
	std::memset(_buffer, 0, BUFFER_SIZE);
}

Response::~Response()
{
	if (_body_fd != -1)
		close(_body_fd);
}

void	Response::formatResponse()
{
	if (_header_sent != 0)
		return;
	std::string	formatted = buildHttpResponse();

	if (formatted.size() > BUFFER_SIZE)
	{
		//Doc NGINX: A request header field cannot exceed the size of one buffer as well
		//, or the 400 (Bad Request) error is returned to the client. Buffers are allocated only on demand.
		//By default, the buffer size is equal to 8K bytes.

		_status_code = INTERNAL_SERVER_ERROR;

		std::cerr << "[Error] Response header size exceed buffer size " << formatted.size() << " bytes" << std::endl;
		formatted = "HTTP/1.1 500 Internal Server Error\r\n"
					"Content-Length: 0\r\n"  //Content-length = error file size
					"Connection: close\r\n"
					"\r\n";
	}

	std::memcpy(_buffer, formatted.c_str(), formatted.size());

	_buffer_size = formatted.size();
	_buffer_sent = 0;
}

std::string	Response::buildHttpResponse()
{
	std::stringstream response;

	response << _http_version << " " << _status_code << " "
			 << httpStatusToString(static_cast<t_HttpCode>(_status_code)) << "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
	it != _headers.end(); it++)
		response << it->first << ": " << it->second << "\r\n";
	response << "\r\n";

	return response.str();
}

bool	Response::readBodyChunk()
{
	if (_body_type == STATIC && _body_fd != -1)
	{
		ssize_t bytes_read = read(_body_fd, _buffer, BUFFER_SIZE);
		if (bytes_read < 0)
		{
			std::cerr << "[Error] Failed to read file: " <<
			strerror(errno) << std::endl;

			close(_body_fd);
			_body_fd = -1;

			_state = DONE;
			return false;
		}
		if (bytes_read == 0)
		{
			close(_body_fd);
			_body_fd = -1;

			_state = DONE;
			return false;
		}
		_buffer_size = bytes_read;
		_buffer_sent = 0;
	}

	// DYNAMIC a implementer

	return true;
}


void	Response::updateBytesSend(size_t bytes_sent)
{
	if (_state == SEND_HEADER)
	{
		_buffer_sent += bytes_sent;
		_header_sent += bytes_sent;
		if (_buffer_sent >= _buffer_size)
		{
			_state++;
			// implementer hasBody() pour savoir si on passe directement a DONE
			resetBuffer();
		}
	}
	else if (_state == SEND_BODY)
	{
		_buffer_sent += bytes_sent;
		_body_sent += bytes_sent;

		if (_buffer_sent >= _buffer_size)
			resetBuffer();
		
		if (_body_sent >= _body_size)
		{
			if (_body_fd != -1)
			{
				close(_body_fd);
				_body_fd = -1;
			}
			_state = DONE;
		}
	}
}

const char*	Response::getDataToSend(size_t& size)
{
	if (_state == SEND_HEADER)
	{
		size = _buffer_size - _buffer_sent;
		return _buffer + _buffer_sent;
	}
	else if (_state == SEND_BODY)
	{
		if (_buffer_size == 0)
		{
			if (!readBodyChunk())
				return NULL;
		}
		size = _buffer_size - _buffer_sent;
		return _buffer + _buffer_sent;
	}
	return NULL;
}

bool	Response::isDone() const
{
	return _state == DONE;
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
	return _body_fd;
}

void	Response::setBodyFd(int fd)
{
	_body_fd = fd;
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
	if (_body_fd != -1)
	{
		close(_body_fd);
		_body_fd = -1;
	}
	_body_size = 0;
	_body_sent = 0;
	_body_content.clear();
	_buffer_size = 0;
	_buffer_sent = 0;
	std::memset(_buffer, 0, BUFFER_SIZE);
}


std::ostream& operator<<(std::ostream& os, const Response& response)
{
	os << "[Response] Status: " << response._status_code 
		<< ", Type: " << response._body_type
	   << ", Body: " << response._body_size << " bytes"
	   << ", FD: " << response._body_fd;
	return os;
}

/* 
	// RESPONSABILITE de Connection -> Send Response au fur et a mesure

	char buffer[8192]; //8KB optimal?
	ssize_t bytesRead;

	while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
		response_body.append(buffer, bytesRead);
	close(fd);
	if (bytesRead < 0)
	{
		_status_code = INTERNAL_SERVER_ERROR;
		return;
	} */
