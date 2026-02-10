#ifndef HTTPUTILS_HPP
#define HTTPUTILS_HPP

#include <iostream>
#include <unistd.h>

namespace httpUtils {

	t_HttpCode errnoToHttpStatus(int error_code);

	std::string	getTime();

	std::string intToString(size_t value); 
}

#endif