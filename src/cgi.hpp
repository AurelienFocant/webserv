
#ifndef		CGI_HPP
# define	CGI_HPP

# include <unistd.h>

class	cgi {
	public:
		bool	createChild(/*name of excec as parameter ?*/);
	private:
		cgi();
		cgi(const cgi& copy_from);
		~cgi();
		cgi&	operator=(const cgi& rhs);
};

#endif



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
