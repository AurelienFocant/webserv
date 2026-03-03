#ifndef HTTPUTILS_HPP
#define HTTPUTILS_HPP

#include <iostream>
#include <unistd.h>
#include "HTTPenum.hpp"

namespace httpUtils {

	t_HttpCode errnoToHttpStatus(int error_code);

	std::string	getTime();

	std::string intToString(size_t value); 
};

#endif