/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 18:50:20 by stempels          #+#    #+#             */
/*   Updated: 2025/12/03 18:58:15 by stempels         ###   ########.fr       */
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

/*MACROS*/

class	Request	{
	public:
	/*Constructor - Copy Constructor - Destructor*/
//		Request();
//		Request(const Request& copy_from);
//		~Request();

	/*Overloaded operators*/
//		Request&	operator=(const Request& other) ;

	/*Publics Methods*/
	bool	parseRequest(std::string request);
	bool	parseRequestLine(std::stringstream& request) ;
	bool	parseHeader(std::stringstream& request) ;

	/*Getters - Setters*/
	bool	setMethod(std::string new_method) { method = new_method; return (1);}
	bool	setRequestUrl(std::string new_url) { request_url = new_url; return (1);}
	bool	setHttpVersion(std::string new_version) { http_version = new_version; return (1);}

	std::string	getMethod() { return(method);}
	std::string	getRequestUrl() { return(request_url);}
	std::string	getHttpVersion() { return(http_version);}

	private:
	/*Private Attributes*/
		std::map<std::string, std::string>		options;
		std::string								type;
		std::string								method;
		std::string								request_url;
		std::string								http_version;
};

#endif
