
#ifndef		CGI_HPP
# define	CGI_HPP

# include "RequestHandler.hpp"

# include <unistd.h>

namespace	cgi {
		bool	execute(const RequestHandler& handler, const Response& response, char** env) ;
		bool	launchCgi(/*name of excec as parameter ?*/);
		char	*findInterpreter(const std::string& extension) ;
		char	**buildCgiEnv(const Request& request);
};

#endif
