#ifndef		RESPONSE_HPP
# define	RESPONSE_HPP

/*INCLUDES*/
#include "Request.hpp"
#include "HTTPenum.hpp"
#include "../Utils/httpUtils.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <cerrno>

class	Response
{
	public:

	friend std::ostream& operator<<(std::ostream& os, const Response& resp);

	/*Constructor - Copy Constructor - Destructor*/
		Response();
		~Response();
	
	enum	state {
		DEFAULT,
		PROCESSING_CGI,
		READY,
		SENDING,
		DONE,
	};

	/* Public Attributes */

	bool				isCGI;
	size_t								_offset;

	/*Publics Methods*/

	void				formatResponse();
	
	const char*			getDataToSend(size_t& to_send);
	void				updateBytesSend(size_t bytes_sent);

	bool				isDefault() const;
	bool				isDone() const;
	bool				isProcessingCGI() const;


	void				cleanResponse();

	/*Setters*/
	void				setState(int state);
	void				setStatusCode(int status_code);
	void				setHttpVersion(const std::string& version);
	void				setMethod(t_method method);
	void				setHeader(const std::string& key, const std::string& value);
	void				setBody(const std::string& content);
	void				addCgiBody(char* content, size_t bytes_read);

	void				setBodySize(int size);

	/*Getters*/
	int					getState() const;
	int					getStatusCode() const;
	std::string			getHttpVersion() const;
	std::string			getHeader(const std::string& key) const;
	std::string			getBody() const;
	int					getBodySize() const;

	private:

	/*Private Attributes*/

	int									_state;
	int									_status_code;
	bool								_isCGI;
	t_method							_method;
	std::string							_http_version;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

	std::string							_data;

	/* Private Methods */

	std::string			buildHttpResponse();

};

#endif
