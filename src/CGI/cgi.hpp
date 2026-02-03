
#ifndef		CGI_HPP
# define	CGI_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "RequestHandler.hpp"

# include <unistd.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>

namespace	cgi {
		bool	execute(const RequestHandler& handler, Response& response, char** env) ;
		bool	launchCgi(char** argv, char** env) ;
		char	*findInterpreter(const std::string& extension) ;
		char	**buildCgiEnv(const Request& request);
};

#endif
