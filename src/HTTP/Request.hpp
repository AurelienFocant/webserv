#ifndef		REQUEST_HPP
# define	REQUEST_HPP

/*INCLUDES*/
# include <cstdlib>
# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>
# include <utility>

# include "HTTPTokenizer.hpp"

/*ENUM*/
# include "HTTPenum.hpp" // Moved enum to their own .hpp, was getting out of hand

/*MACROS*/

class	Request	: private HTTPTokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Request();
		Request(std::string const& request);
		~Request() {};

	/*Overloaded operators*/
//		Request&	operator=(const Request& other) ;

	/*Publics Methods*/
		bool			parseRequest();
		bool			handleBody(unsigned int max_body);
		bool			cleanRequest();
		bool			addInput(const std::string& input) ;

	/*Setters - Getters*/
		t_method		getMethod() const ;
		std::string		getRequestUri() const ;
		std::string		getHttpVersion() const ;
		std::string		getBody() const ;
		bool			isCompleted() const ;
		t_HttpCode		getStatusCode() const ;
		const int&		getState() const ;

		std::vector<std::string>	getHeaderValues(std::string header_name) const ;
		const std::multimap<std::string, std::string>&	getHeaders() const ;

	private:
		Request(const Request& copy_from);
	/*Private Attributes*/
		//Request State
		int										_progress;
		bool									_complete;
		t_HttpCode								_status_code;
		bool									(Request::*_body_handler)(unsigned int max_body);

		//Request informations
		t_method								_method;
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
//		t_method	idMethod(std::string& method) ;
		void	parseFirstLine() ;
		void	extractFirstLineInfo() ;
		void	parseHeader() ;
		bool	setupBodyHandler() ;

		bool	isFirstLineValid() ; 
		bool	extractHeadersInformations() ;
		bool	areHeadersValid() const ;
		bool	areMandatoryHeadersPresent() const ;
		bool	isUniqueHeader(const std::string& header_key, const char** unique_list) const;

		bool	defineBodyExtractionHandler() ;
		bool	bodyHandlerTransfertEncoding(unsigned int max_body) ;
		bool	bodyHandlerContentLength(unsigned int max_body) ;
		bool	bodyHandlerMultipart() ;

		std::string	normalizeHeadersKey(std::string argument) const ;
		void	safeInsertion(const std::string& key, const std::string& value) ;
		void	detectImportantValue(std::string& argument, std::string value) ;

};

std::ostream&	operator<<(std::ostream& ostream, Request& other) ;
std::ostream&	operator<<(std::ostream& ostream, std::vector<t_Token>& other) ;

#endif
