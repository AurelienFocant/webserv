#ifndef		HANDLEUSER_HPP
# define	HANDLEUSER_HPP

# include <string>
# include <map>
# include <vector>
# include "RequestHandler.hpp"

namespace handleUser {
	bool	createNewUser(const RequestHandler& handler);
	bool	deleteUser();
}

#endif
