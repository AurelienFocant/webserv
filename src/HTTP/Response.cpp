#include "Response.hpp"
	
Response::Response()
: _state(DEFAULT)
, _body_type(STATIC)
, _status_code(OK)
, _http_version("")
, _header_sent(0)
, _body_fd(-1)
, _body_content("")
, _body_size(0)
, _body_sent(0)
, _bytes_in_buffer(0)
, _buffer_offset(0)
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

	_state = SEND_HEADER;
	
	if (formatted.size() > BUFFER_SIZE)
	{
		//Doc NGINX: A request header field cannot exceed the size of one buffer as well
		//, or the 400 (Bad Request) error is returned to the client. Buffers are allocated only on demand.
		//By default, the buffer size is equal to 8K bytes.

		_status_code = INTERNAL_SERVER_ERROR;

		std::cerr << "[Error] Response.cpp::39 Response header size exceed buffer size " << formatted.size() << " bytes" << std::endl;
		formatted = "HTTP/1.1 500 Internal Server Error\r\n"
					"Content-Length: 0\r\n"  //Content-length = error file size
					"Connection: close\r\n"
					"\r\n";
	}

	std::memcpy(_buffer, formatted.c_str(), formatted.size());

	_bytes_in_buffer = formatted.size();
	_buffer_offset = 0;
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
	switch (_body_type)
	{
		case STATIC:
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
			_bytes_in_buffer = bytes_read;
			_buffer_offset = 0;
			break ;
		}
		case DYNAMIC:
		{
			// DYNAMIC a implementer
			break;
		}
		case MEMORY:
		{
			size_t remaining = _body_content.size() - _body_sent;
			if (remaining == 0)
			{
				_state = DONE;
				return false;
			}
			size_t to_copy = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
			std::memcpy(_buffer, _body_content.c_str() + _body_sent, to_copy);

			_bytes_in_buffer = to_copy;
			_buffer_offset = 0;
			_body_sent += to_copy;
			break;
		}
	}

	return true;
}

void	Response::updateBytesSend(size_t bytes_sent)
{
	if (_state == SEND_HEADER)
	{
		_buffer_offset += bytes_sent;
		_header_sent += bytes_sent;
		if (_buffer_offset >= _bytes_in_buffer)
		{
			resetBuffer();
			if (_body_size == 0) // if NO body
			{
				_state = DONE;
				return;
			}
			_state = SEND_BODY;
		}
	}
	else if (_state == SEND_BODY)
	{
		_buffer_offset += bytes_sent;
		_body_sent += bytes_sent;

		if (_buffer_offset >= _bytes_in_buffer)
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
		size = _bytes_in_buffer - _buffer_offset;
		return _buffer + _buffer_offset;
	}
	else if (_state == SEND_BODY)
	{
		if (_bytes_in_buffer == 0)
		{
			if (!readBodyChunk())
				return NULL;
		}
		size = _bytes_in_buffer - _buffer_offset;
		return _buffer + _buffer_offset;
	}
	return NULL;
}

bool	Response::isDefault() const
{
	return _state == DEFAULT;
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


std::string	Response::getBodyContent(size_t& size) const
{
	size = _body_size;
	return _body_content;
}

void	Response::setBodyContent(const std::string& content)
{
	_body_type = MEMORY;
	_body_content = content;
	_body_sent = 0;
}

bodyType	Response::getBodyType() const
{
	return _body_type;
}

void	Response::setBodyType(bodyType type)
{
	_body_type = type;
}

void	Response::resetBuffer()
{
	_bytes_in_buffer = 0;
	_buffer_offset = 0;
	std::memset(_buffer, 0, BUFFER_SIZE);
}

void	Response::cleanResponse()
{
	_state = DEFAULT;
	_body_type = STATIC;
	_status_code = OK;
	_http_version = "";

	_headers.clear();
	_header_sent = 0;
	if (_body_fd != -1)
	{
		close(_body_fd);
		_body_fd = -1;
	}
	_body_size = 0;
	_body_sent = 0;
	_body_content.clear();
	_bytes_in_buffer = 0;
	_buffer_offset = 0;
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
