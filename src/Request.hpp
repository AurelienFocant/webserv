/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 18:50:20 by stempels          #+#    #+#             */
/*   Updated: 2026/01/07 17:03:09 by stempels         ###   ########.fr       */
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
/*ENUM*/
# include "enum.hpp" // Moved enum to their own .hpp, was getting out of hand

/*MACROS*/

class	Request	: private HTTPTokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Request();
		Request(std::string const& request);
//		Request(const Request& copy_from);
		~Request() {std::cout << "Destructor called: Request" << std::endl;};

	/*Overloaded operators*/
//		Request&	operator=(const Request& other) ;

	/*Publics Methods*/
		bool			parseRequest();
		bool			cleanRequest();
		bool			addInput(std::string input) ;

	/*Setters - Getters*/
		t_Method		getMethod() const { return(_method);}
		std::string		getRequestUri() const { return(_request_uri);}
		std::string		getHttpVersion() const { return(_http_version);}
		std::string		getBody() const ;
		bool			getCompleted() const { return(_complete);}
		t_HttpCode		getStatusCode() const { return(_status_code);}

		const std::multimap<std::string, std::string>&	getHeadersValue() const {return (_headers);}

	private:
	/*Private Attributes*/
		//Request State
		int										_progress;
		bool									_complete;
		t_HttpCode								_status_code;
		bool									(Request::*_body_handler)( void );

		//Request informations
		t_Method								_method;
		std::string								_request_uri;
		std::string								_http_version;
		std::string								_body;

		//Request usefull header informations
		std::multimap<std::string, std::string>	_headers;
		size_t									_nbr_headers;
		bool									_content_encoding;
		std::string								_content_type;
		size_t									_content_length;
        // + Connection, Host, etc etc ?

		//Http methods limits -> assigned in Request.cpp
		static const std::string				authorized_method;
		static const std::string				unimplemented_method;
		static const char*		 				important_argument[];

		//std::vector<t_Token>::const_iterator	_list_it;

	/*Private Methods*/
		t_Method	idMethod(std::string& method) ;
		bool	setMethod() ;
		bool	setRequestUri() ;
		bool	setHttpVersion() ;
		bool	parseFirstLine() ;
		bool	parseHeader() ;
		bool	handleBody() ;
		bool	extractHeadersInformations() ;

		bool	defineBodyExtractionHandler() ;
		bool	bodyHandlerTransfertEncoding() ;
		bool	bodyHandlerContentLength() ;
		bool	bodyHandlerMultipart() ;

		std::string	normalizeHeadersKey(std::string argument) ;
		void	safeInsertion(const std::string& key, const std::string& value) ;
		void	detectImportantValue(std::string& argument, std::string value) ;

		enum	progress {
			PARSER_ERROR = -1,
			START = 0,
			METHOD,
			URI,
			VERSION,
			FIRST_LINE,
			PARSED,
			BODY_HANDLING,
			DONE,
		};
};

std::ostream&	operator<<(std::ostream& ostream, Request& other) ;
std::ostream&	operator<<(std::ostream& ostream, std::vector<t_Token>& other) ;
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
