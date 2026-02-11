
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
		int		*launchCgi(char** argv, char** env) ;
		char*	findInterpreter(const t_extension& extension) ;
		char	**buildCgiEnv(const RequestHandler& handler);
		char*	convertStringToChar(const std::string& string) ;
};

#endif
