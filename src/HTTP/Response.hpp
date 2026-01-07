#ifndef		RESPONSE_HPP
# define	RESPONSE_HPP

/*INCLUDES*/
#include "Request.hpp"

class	Response
{
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Response();
		Response(std::string const& Response);
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


	/*Setters - Getters*/

	private:

	/*Private Attributes*/
	//	state			_state;
	//	t_HttpCode		_status_code;
	//	bodyType		_type;

		//std::string		_headers;
		//size_t			_headers_sent;
/*		
		int				_fd;
		size_t			_body_size;
		size_t			_body_sent;
		char			_buffer[8192]; // or std::vector<char> ? for CGI?
		size_t			_buffer_sent;
		size_t			_buffer_size;
*/

	/*Private Methods*/


};

#endif
