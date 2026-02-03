#ifndef		HANDLEUSER_HPP
# define	HANDLEUSER_HPP

# include <string>
# include <map>
# include <vector>
# include <iostream>
# include <sstream>
# include <fstream>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/types.h>
# include "Request.hpp"
# include "RequestHandler.hpp"

namespace handleUser {
	t_HttpCode	createNewUser(const RequestHandler& handler);
	t_HttpCode	deleteUser(const Request& request);
}

#endif
