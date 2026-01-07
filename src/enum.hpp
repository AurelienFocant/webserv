/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enum.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:59:07 by stempels          #+#    #+#             */
/*   Updated: 2026/01/07 17:06:50 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		ENUM_HPP
# define	ENUM_HPP

typedef	enum s_method {		//Enum for HTTP METHOD
	NOT_SET = -1,
	GET = 0,
	POST,
	DELETE,
	UNKNOWN,
}		t_Method;

typedef	enum s_progress {	//Enum for request traitment progression
			PARSER_ERROR = -1,
			START = 0,
			METHOD,
			URI,
			VERSION,
			FIRST_LINE,
			PARSED,
			BODY_HANDLING,
			DONE,
}		t_progress;

typedef enum s_httpCode {	//Enum for HTTP Status codes
	// ERROR_TYPE = 			Error_code	http version
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

#endif
