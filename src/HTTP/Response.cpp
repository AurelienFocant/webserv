#include "Response.hpp"
	
Response::Response()
: isCGI(false)
, _offset(0)
, _state(DEFAULT)
, _status_code(OK)
, _method(NOT_SET)
{}

Response::~Response() {}

void	Response::formatResponse()
{
	if (_state != READY)
		return;

	_headers["Content-Length"] = httpUtils::intToString(_body.size());

	_data = buildHttpResponse();

	std::string res = buildHttpResponse();

	if (_method != HEAD)
		_data += _body;

	_state = SENDING;
}

std::string	Response::buildHttpResponse()
{
	std::stringstream response;

	response << _http_version << " " << _status_code << " "
			 << httpStatusToString(static_cast<t_HttpCode>(_status_code)) << "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
	it != _headers.end(); ++it)
		response << it->first << ": " << it->second << "\r\n";
	
	response << "\r\n";
	return response.str();
}

const char*	Response::getDataToSend(size_t& to_send)
{
	if (_state != SENDING || _offset >= _data.size())
	{
		to_send = 0;
		return NULL;
	}
	to_send = _data.size() - _offset;
	return _data.c_str() + _offset;
}

void	Response::updateBytesSend(size_t bytes_sent)
{
	if (_state != SENDING)
		return;
	
	_offset += bytes_sent;

	if (_offset >= _data.size())
		_state = DONE;
}

/* STATES CHECK */

bool	Response::isDefault() const
{
	return _state == DEFAULT;
}

bool	Response::isDone() const
{
	return _state == DONE;
}

bool	Response::isProcessingCGI() const
{
	return _state == PROCESSING_CGI;
}

/* GETTERS / SETTERS */

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

void	Response::setHttpVersion(const std::string& version)
{
	_http_version = version;
}

std::string	Response::getHttpVersion() const
{
	return _http_version;
}

void	Response::setMethod(t_method method)
{
	_method = method;
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

std::string	Response::getBody() const
{
	return _body;
}

void	Response::setBody(const std::string& content)
{
	_body = content;
}

void	Response::addCgiBody(char* content) {
	if (content)
		_body += content;
	return ;
}

/* RESET */

void	Response::cleanResponse()
{
	_state = DEFAULT;
	_status_code = OK;
	_http_version = "";
	_headers.clear();
	_body.clear();
	_data.clear();
	isCGI = false;
	_offset = 0;
}

/* DEBUG */

std::ostream& operator<<(std::ostream& os, const Response& response)
{
	os << "[Response] Status: " << response._status_code 
	   << " | Body: " << response._body.size() << " bytes"
	   << " | Sent: " << response._offset << "/" << response._data.size();
	return os;
}
