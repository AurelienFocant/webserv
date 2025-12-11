/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 18:50:20 by stempels          #+#    #+#             */
/*   Updated: 2025/12/11 16:44:26 by afocant          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		REQUEST_HPP
# define	REQUEST_HPP

/*INCLUDES*/
# include <cstdlib>
# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <utility>

# include "HTTPTokenizer.hpp"
/*MACROS*/

/*ENUM*/
typedef enum // ERROR_TYPE = Error_code	http version
{
	INIT_STATE						= -1,

    // 1xx — Informational
    CONTINUE                        = 100,	 // 1.1
    SWITCHING_PROTOCOLS            	= 101,	 // 1.1

    // 2xx — Successful
    OK                              = 200,   // 1.0
    CREATED                         = 201,   // 1.0
    ACCEPTED                        = 202,   // 1.0
    NON_AUTHORITATIVE_INFORMATION   = 203,   // 1.1
    NO_CONTENT                      = 204,   // 1.0
    RESET_CONTENT                   = 205,   // 1.1
    PARTIAL_CONTENT                 = 206,   // 1.1

    // 3xx — Redirection
    MULTIPLE_CHOICES                = 300,   // 1.0
    MOVED_PERMANENTLY               = 301,   // 1.0
    FOUND                           = 302,   // 1.0
    SEE_OTHER                       = 303,   // 1.0
    NOT_MODIFIED                    = 304,   // 1.0
    USE_PROXY                       = 305,   // 1.0
    TEMPORARY_REDIRECT              = 307,   // 1.1

    // 4xx — Client Error
    BAD_REQUEST                     = 400,   // 1.0
    UNAUTHORIZED                    = 401,   // 1.0
    PAYMENT_REQUIRED                = 402,   // 1.0
    FORBIDDEN                       = 403,   // 1.0
    NOT_FOUND                       = 404,   // 1.0
    METHOD_NOT_ALLOWED              = 405,   // 1.1
    NOT_ACCEPTABLE                  = 406,   // 1.1
    PROXY_AUTHENTICATION_REQUIRED   = 407,   // 1.1
    REQUEST_TIMEOUT                 = 408,   // 1.1
    CONFLICT                        = 409,   // 1.1
    GONE                            = 410,   // 1.1
    LENGTH_REQUIRED                 = 411,   // 1.1
    PRECONDITION_FAILED             = 412,   // 1.1
    REQUEST_ENTITY_TOO_LARGE        = 413,   // 1.1
    REQUEST_URI_TOO_LONG            = 414,   // 1.1
    UNSUPPORTED_MEDIA_TYPE          = 415,   // 1.1
    REQUESTED_RANGE_NOT_SATISFIABLE = 416,   // 1.1
    EXPECTATION_FAILED              = 417,   // 1.1

    // 5xx — Server Error
    INTERNAL_SERVER_ERROR           = 500,   // 1.0
    NOT_IMPLEMENTED                 = 501,   // 1.0
    BAD_GATEWAY                     = 502,   // 1.0
    SERVICE_UNAVAILABLE             = 503,   // 1.0
    GATEWAY_TIMEOUT                 = 504,   // 1.1
    HTTP_VERSION_NOT_SUPPORTED      = 505    // 1.1
}									t_HttpCode;

class	Request	: public HTTPTokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Request();
		Request(std::string const& request);
//		Request(const Request& copy_from);
		~Request() {std::cout << "Destructor called: Request" << std::endl;};

	/*Overloaded operators*/
//		Request&	operator=(const Request& other) ;

	/*Publics Methods*/
		bool	parseRequest();

	/*Getters*/
		std::string		getMethod() const { return(method);}
		std::string		getRequestUrl() const { return(request_url);}
		std::string		getHttpVersion() const { return(http_version);}
		bool			getCompleted() const { return(complete);}
		t_HttpCode		getStatusCode() const { return(status_code);}

		const std::multimap<std::string, std::string>&	getOptions() const {return (options);}

	private:
	/*Private Attributes*/
		//Request State
		bool									complete;
		t_HttpCode								status_code;

		//Request informations
		std::string								type;
		std::string								method;
		std::string								request_url;
		std::string								http_version;
		std::string								html_body;          // rename to body (could be json)

		//Request usefull header informations
		std::multimap<std::string, std::string>	options;            // rename to headers
		size_t									content_length;
        // + Connection, Host, etc etc ?

		//Http methods limits -> assigned in Request.cpp
		static const std::string				authorized_method;
		static const std::string				unimplemented_method;
		static const char*		 				important_argument[];

	/*Private Methods*/
		bool	setMethod(std::vector<t_Token>::const_iterator& it) ;
		bool	setRequestUrl(std::vector<t_Token>::const_iterator& it) ;
		bool	setHttpVersion(std::vector<t_Token>::const_iterator& it) ;

		std::string	normalizeOptions(std::string argument) ;
		void	detectImportantValue(std::string& argument, std::string value) ;
};

std::ostream&	operator<<(std::ostream& ostream, Request& other) ;
std::string		httpStatusToString(t_HttpCode code) ;
t_HttpCode httpStatusFromString(const std::string& s) ;

#endif

// #ifndef REQUEST_HPP
// #define REQUEST_HPP
//
// #include <iostream>
//
// class Request
// {
// 	private:
//
// 	int _state; // complete, incomplete, error
// 	bool _connection;
// 	int	_statusCode; //if state = error (431, 411, 413 400...);
//
// 	/* request line */
// 	std::string _method; 
// 	std::string _requestUri;
// 	std::string _version;
//
// 	public:
//
// 	void	hasError(); //
// 	void	isComplete(); //-> change to EPOLLOUT->processRequest()->buildResponse();
//
// 	const std::string&	getState() const;
// 	const std::string&	getStatusCode() const;
//
// 	void	setStatusCode(int code);
//
//
// 	const std::string& getMethod() const;
// 	const std::string& getRequestUri() const;
//
//
//
// };
//
// processRequest()
// 	-> validMethod
// 	-> validLocations
// 	-> methodHandlers
//
// std::string statusText;
//
//
// class Response
// {
// 	private:
//
// 	buildStatusLine(const Request& Request);
// 	buildHeader(const Request& Request);
// 		Server:
// 		Date:
// 		Content-Lenght:
// 		Content-Type:
// 		Connection:
// 	buildContent();
//
// 	public: 
//
// 	buildResponse();
//
// };
//
//
// #endif
