/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:00:18 by stempels          #+#    #+#             */
/*   Updated: 2025/12/09 11:28:33 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

const std::string	Request::authorized_method = "GET POST";
const std::string	Request::unimplemented_method = "HEAD";

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
/*	if ((*it)._tkType != EOL)	
		return (false);
*/
	//Parsing header
	while (it->_tkType == WORD) {
		std::string	options_name = it->_lexeme;
		it++;
		if (it->_tkType == COLON) {
			it++;
			if (it->_tkType == WORD) {
				while (it->_tkType == WORD) {
					options.insert(std::make_pair(options_name, it->_lexeme));		
					it++;
					if (it->_tkType != COMA)
						break ;
					it++;
				}
			}
			else
				return (false);
		}
		else
			return (false);
		if (it->_tkType == EOL)
			it++;
	}
	if (it->_tkType != EOL)
		return (false);
	return (true);
}

bool	Request::setMethod(const t_Token& token) {
	if (token._tkType != WORD)	
		return (false);
	if (authorized_method.find(token._lexeme) != -1) {
		method = token._lexeme;
		return (true);
	}
	if (unimplemented_method.find(token._lexeme) != -1) {
		method = "Error";
		return (false);
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
	ostream << other.getMethod() << '\t' << other.getRequestUrl() << '\t' << other.getHttpVersion() << '\n';
	for (std::multimap<std::string, std::string>::const_iterator	it = other.getOptions().begin(); it != other.getOptions().end(); it++) {
		std::cout << it->first << ' ' << it->second << '\n';
	}
	return (ostream);
}
