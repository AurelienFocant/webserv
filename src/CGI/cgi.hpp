
#ifndef		CGI_HPP
# define	CGI_HPP

# include "Webserv.hpp"
# include "Connection.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "RequestHandler.hpp"

# include <unistd.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>

namespace	cgi {
		bool		execute(const RequestHandler& handler, Connection& conn, char** env) ;
		bool		launchCgi(Connection& conn, char** argv, char** env) ;
		char		**buildCgiEnv(const RequestHandler& handler) ;
		char*		convertStringToChar(const std::string& string) ;
		char*		findInterpreter(const t_extension& extension) ;
		bool		parseOutput(Response& response);

};

#endif
