#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

class Request
{
	private:

	int _state; // complete, incomplete, error
	bool _connection;
	int	_statusCode; //if state = error (431, 411, 413 400...);

	/* request line */
	std::string _method; 
	std::string _requestUri;
	std::string _version;

	public:

	void	hasError(); //
	void	isComplete(); //-> change to EPOLLOUT->processRequest()->buildResponse();

	const std::string&	getState() const;
	const std::string&	getStatusCode() const;

	void	setStatusCode(int code);


	const std::string& getMethod() const;
	const std::string& getRequestUri() const;



};

processRequest()
	-> validMethod
	-> validLocations
	-> methodHandlers

std::string statusText;


class Response
{
	private:

	buildStatusLine(const Request& Request);
	buildHeader(const Request& Request);
		Server:
		Date:
		Content-Lenght:
		Content-Type:
		Connection:
	buildContent();

	public: 

	buildResponse();

};



#endif
