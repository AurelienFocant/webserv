/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:00:18 by stempels          #+#    #+#             */
/*   Updated: 2025/12/08 16:46:05 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*Constructor - Copy Constructor - Destructor*/
Request::Request(std::vector<t_Token>& token_list) {
	if (!parseRequest(token_list)) {
		std::cerr << "Wrong Request" << std::endl;
	}
}

/*Public Methods*/
bool	Request::parseRequest(std::vector<t_Token>& token_list) {
	std::vector<t_Token>::const_iterator	it = token_list.begin();

	//Parsing for mandatory first line information
	if (!setMethod(*it))
		return (false);
	it++;
	if (!setRequestUrl(*it))
		return (false);
	it++;
	if (!setHttpVersion(*it))
		return (false);
	it++;
	if ((*it)._tkType != EOL)	
		return (false);
	return (true);
}

bool	Request::setMethod(const t_Token& token) {
	if (token._tkType != WORD)	
		return (false);
	if (!token._lexeme.compare("GET")) {
		method = "GET";
		return (true);
	}
	return (false);
}

bool	Request::setRequestUrl(const t_Token& token) {
	if (token._tkType != WORD)
		return (false);
	if (token._lexeme[0] == '/') {
		request_url = token._lexeme;
		return (true);
	}
	return (false);
}

bool	Request::setHttpVersion(const t_Token& token) {
	if (token._tkType != WORD)
		return (false);
	if (!token._lexeme.compare("HTTP/1.0") || !token._lexeme.compare("HTTP/1.1")) {
		http_version = token._lexeme;
		return (true);
	}
	return (false);
}

std::ostream&	operator<<(std::ostream& ostream, Request& other) {
	ostream << other.getMethod() << '\t' << other.getRequestUrl() << '\t' << other.getHttpVersion();
	return (ostream);
}
