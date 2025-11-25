/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 18:50:20 by stempels          #+#    #+#             */
/*   Updated: 2025/11/25 19:25:17 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		REQUEST_HPP
# define	REQUEST_HPP

/*INCLUDES*/
# include <iostream>
# include <sting>
# include <map>

/*MACROS*/

class	Request	{
	public:
	/*Constructor - Copy Constructor - Destructor*/
		Request();
		Request(const Request& copy_from);
		~Request();

	/*Overloaded operators*/
		Request&	operator=(const Request& other) ;

	/*Publics Methods*/
	bool	parseStartLine(std::string request) ;
	/*Getters - Setters*/

	private:
	/*Private Attributes*/
		map<std::string, std::string>	options;
		std::string						type;
};

#endif

#include "Request.hpp"

Request::Request() {
}


/*Public Methods*/
bool	Request::parseStartLine(std::string request) {
	int			pos = 0;
	Request		Message;

	if (pos = request.find("\r\n")) {
		std::string	start_line = substr(0, pos);
		request.erase(0, start_line.size + 2);
	}
}
