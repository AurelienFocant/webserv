/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stempels <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:00:18 by stempels          #+#    #+#             */
/*   Updated: 2025/12/04 16:00:05 by stempels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*Public Methods*/
bool	Request::parseRequest(std::string request) {
	std::stringstream	stream;

	stream << request;
	parseRequestLine(stream);
	parseHeader(stream);
	return (0);
}

bool	Request::parseRequestLine(std::stringstream& request) {
	int			pos = 0;

	char*				buffer;
	std::string			token;

	request >> token;
	setMethod(token);
	request >> token;
	setRequestUrl(token);
	request >> token;
	setHttpVersion(token);
	request.getline(buffer, 60);
	std::cout << getMethod() << " " << getRequestUrl() << " " << getHttpVersion() << std::endl;
	return (1);
}

bool	Request::parseHeader(std::stringstream& request) {
	std::string	token = "1";
	std::string	test1 = "Yolo";
	std::string	test2 = "Halaa";
	char	buffer_token[256];
	char	buffer_arg[256];

	int	i = 2;
	while (i && token.length() && token != "\r\n\r\n") {	
		request.getline(buffer_token, 60, ':');
		request.getline(buffer_arg, 256, '\r');
		options[buffer_token] = buffer_arg;
		//this->options.insert(std::make_pair("Hello", "World!"));
		//		this->options.insert(test1, test2);
		i--;
	}
	for (std::map<std::string, std::string>::const_iterator it = options.begin(); it != options.end(); ++it) {
	      std::cout << "{" << it->first << " = " << it->second << "}" << std::endl;
	}
	return (1);
}
