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

//Implemented methods
// GET, POST, DELETE, HEAD --> see t_method enum(HTTPenum.hpp)

static const char*		methods[] = {
	"GET", "POST", "DELETE", "HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE"
	, NULL};

static const char*		uniqueHeadersHttp_0[] = {
	"CONTENT_LENGTH"
	, NULL};

static const char*		uniqueHeadersHttp_1[] = { 
	"CONTENT_LENGTH"
	, NULL};

static const char*		mandatoryHeadersHttp_0[] = {
	NULL};

static const char*		mandatoryHeadersHttp_1[] = {
	"HOST"
	, NULL};

static const char*	important_argument[] = {
	"CONTENT_LENGTH", "TRANSFER_ENCODING"
	, NULL};

class	Request	: private HTTPTokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Request();
		Request(std::string const& request);
		~Request() {};

	/*Publics Methods*/
		bool			parseRequest();
		bool			handleBody(unsigned int max_body);
		bool			cleanRequest();
		bool			addInput(const std::string& input) ;
		void			sanitizeInput(std::string& input);

	/*Setters - Getters*/
		void				setComplete(bool status);
		void				setStatusCode(t_HttpCode status_code);
		t_method			getMethod() const ;
		std::string			getRequestUri() const ;
		std::string			getHttpVersion() const ;
		const std::string&	getBody() const ;
		bool				isCompleted() const ;
		t_HttpCode			getStatusCode() const ;
		const int&			getState() const ;
		size_t				getContentLength() const ;

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

	/*Private Methods*/
		void	parseFirstLine() ;
		void	extractFirstLineInfo() ;
		void	parseHeader() ;
		bool	setupBodyHandler() ;

		bool	isFirstLineValid() ; 
		bool	extractHeadersInformations() ;
		bool	areHeadersValid() const ;
		bool	areMandatoryHeadersPresent() const ;
		bool	isUniqueHeader(const std::string& header_key, const char** unique_list) const;
		bool	detectImportantValue(std::string& argument, std::string value) ;
		void	safeInsertion(const std::string& key, const std::string& value) ;

		bool	defineBodyExtractionHandler() ;
		bool	bodyHandlerTransfertEncoding(unsigned int max_body) ;
		bool	bodyHandlerContentLength(unsigned int max_body) ;
		bool	bodyHandlerMultipart() ;

		std::string	normalizeHeadersKey(std::string argument) const ;

};

std::ostream&	operator<<(std::ostream& ostream, Request& other) ;
std::ostream&	operator<<(std::ostream& ostream, std::vector<t_Token>& other) ;

#endif
