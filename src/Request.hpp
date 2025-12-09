/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 18:50:20 by stempels          #+#    #+#             */
/*   Updated: 2025/12/09 14:57:02 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		REQUEST_HPP
# define	REQUEST_HPP

/*INCLUDES*/
# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <utility>

# include "HTTPTokenizer.hpp"
/*MACROS*/

class	Request	: virtual private HTTPTokenizer {
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Request();
		Request(std::string request);
//		Request(const Request& copy_from);
//		~Request();

	/*Overloaded operators*/
//		Request&	operator=(const Request& other) ;

	/*Publics Methods*/
	/*Publics Attributes*/
		bool	valid;
		int		status_code;

	/*Getters - Setters*/
		bool	setMethod(const t_Token& token) ;
		bool	setRequestUrl(const t_Token& token) ;
		bool	setHttpVersion(const t_Token& token) ;

		std::string	getMethod() { return(method);}
		std::string	getRequestUrl() { return(request_url);}
		std::string	getHttpVersion() { return(http_version);}
		const std::multimap<std::string, std::string>&	getOptions() {return (options);}

	private:
	/*Private Attributes*/
		std::multimap<std::string, std::string>		options;
		std::string								type;
		std::string								method;
		std::string								request_url;
		std::string								http_version;

	/*Private Methods*/
		bool	parseRequest();
	static const std::string	authorized_method;
	static const std::string	unimplemented_method;
};

std::ostream&	operator<<(std::ostream& ostream, Request& other) ;

#endif
