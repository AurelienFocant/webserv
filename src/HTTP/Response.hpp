#ifndef		RESPONSE_HPP
# define	RESPONSE_HPP

/*INCLUDES*/
#include "Request.hpp"
#include "HTTPenum.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#define BUFFER_SIZE 8192

class	Response
{
	public:

	friend std::ostream& operator<<(std::ostream& os, const Response& resp);

	/*Constructor - Copy Constructor - Destructor*/
		Response();
		~Response();
	
	enum	state {
		SEND_HEADER,
		SEND_BODY,
		DONE,
	};

	enum	bodyType {
		STATIC,
		DYNAMIC,
	};

	/*Publics Methods*/

	void				formatResponse();
	
	const char*			getDataToSend(size_t& size);
	void				updateBytesSend(size_t bytes_sent);
	bool				readBodyChunk();
	bool				isDone() const;

	void				cleanResponse();


	/*Setters - Getters*/
	void				setState(int state);
	void				setStatusCode(int status_code);
	void				setBodyFd(int fd);
	void				setBodySize(int size);
	void				setHttpVersion(const std::string& version);
	void				setHeader(const std::string& key, const std::string& value);
	void				setBodyContent(const std::string& content); // for autoindex

	int					getState() const;
	int					getStatusCode() const;
	int					getBodyFd() const;
	int					getBodySize() const;
	std::string			getHttpVersion() const;
	std::string			getHeader(const std::string& key) const;
	//std::string			getBodyContent() const;

	private:

	/*Private Attributes*/

	int									_state;
	bodyType							_body_type;
	int									_status_code;
	std::string							_http_version;

	std::map<std::string, std::string>	_headers;
	//size_t								_headers_size;
	size_t								_header_sent;
	
	int									_body_fd;
	std::string							_body_content;
	size_t								_body_size;
	size_t								_body_sent;

	char								_buffer[BUFFER_SIZE]; // sert pour header et body | or std::vector<char> ? for CGI?
	size_t								_buffer_size;
	size_t								_buffer_sent;

	/* Private Methods */

	std::string			buildHttpResponse();
	void				resetBuffer();

};


#endif
