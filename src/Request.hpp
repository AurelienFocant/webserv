/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 18:50:20 by stempels          #+#    #+#             */
/*   Updated: 2025/12/09 11:23:24 by stempels         ###   ########.fr       */
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

class	Request	{
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Request(std::vector<t_Token>& token_list);
//		Request(const Request& copy_from);
//		~Request();

	/*Overloaded operators*/
//		Request&	operator=(const Request& other) ;

	/*Publics Methods*/

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
		bool	parseRequest(std::vector<t_Token>& token_list);
	static const std::string	authorized_method;
	static const std::string	unimplemented_method;
};

std::ostream&	operator<<(std::ostream& ostream, Request& other) ;

#endif
