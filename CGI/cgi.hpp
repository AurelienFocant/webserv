
#ifndef		CGI_HPP
# define	CGI_HPP

# include <unistd.h>

class	Cgi {
	public:
	/*Constructors - Copy Constructor - Destructor*/
		Cgi();
		Cgi(enum	script_t, const Request& request, const char*	path_to_script);
		Cgi(enum	script_t, const Request& request, const std::string& path_to_script);
		Cgi(const Cgi& copy_from);
		~Cgi();
	
	/*Overloaded Operator*/
		Cgi&	operator=(const Cgi& rhs);

	
		bool	createChild(/*name of excec as parameter ?*/);
	private:
};

#endif


# include "Cgi.hpp"

/*Constructors*/
Cgi::Cgi() {}
Cgi(enum	script_t, const Request& request, const char*	path_to_script) {
}

Cgi(enum	script_t, const Request& request, const std::string& path_to_script) {
}


bool	cgi::createChild() {
//Pipe creation for communication with the process
	int	pipe_fd[2];
	if (pipe(pipe_fd) < 0) {
		return (false);
	}

//Creation of the subprocess
	pid_t	pid = fork();
	if (pid < 0)
		return (false);
	else if (pid == 0) { // Child process
	//Setup the pipe to write from the child
		close(pipe_fd[1]);
		dup2(pipe_fd[0], STDIN_FILENO);
		close(pipe_fd[0]);

	//DO CHILD STUFF
		std::execv();
		exit(EXIT_FAILURE);
	}
	else { // Parent process
	//Setup the pipe to listen in the parent
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDIN_FILENO);
		close(pipe_fd[1]);

	//DO PARENT STUFF
	}
	return (true);
}
