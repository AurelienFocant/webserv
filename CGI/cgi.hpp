
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

	
		bool	launchCgi(/*name of excec as parameter ?*/);
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


bool	cgi::execute() {
	char** argv;	
	//Create argv for child exec
	try {
		argv = new[](sizeof(char*) * (2 + 1)); //size must depend of number of argument and if an interpreter is needed
		argv[0] = findInterpreter(file_to_execute).c_str;
		argv[1] = file_to_execute.c_str(); //->maybe do one function that initialize whole argv based on file_to_execute
	}
	catch (std::exception& e) {
		//setup Response status code to Internal_server_error: here or up the stack
		std::cerr << "Fatal error; " << e.what() << std::endl;
		return (false); //Continue or crash the program ?
	}
	//Create and initialize environement variable for script
	CppEnv	cgi_env(/*request*/);

	if (!launchCgi(argv, cgi_env.getCEnv())) {
		//some errors happened, setup response code accordingly
	}
	delete[](argv);
	//delete[](cgi_env); -> probably needed, depend on CppEnv implementation
	return (true);
}

bool	cgi::launchCgi(char** argv, char** env) {
//Pipe creation for communication with the child
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

		std::execv(argv[0], argv, env);
		exit(EXIT_FAILURE);
	}
	else { // Parent process
	//Setup the pipe to listen in the parent
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDIN_FILENO);
		close(pipe_fd[1]);
	}


//	Need to read child production ?on std::cout?
	int	status = 0;
	int time = 3000;
	while (time > 0) {
		int	ret = waitpid(pid, &status, WNOHANG);
		switch (waitpid(ret) {
			case (pid):
				break ;
			case (0):
				time -= 50;
				if (usleep(50) < 0) {
					kill(pid);
					//setStatus code internal error
					return (false);
				}
				break ;
			default:
				switch (errno) {
					case (ECHILD):
						break ;
					case (EINTR):
						break ;
					case (EINVAL):
						break ;
					default:
						//SOMETHING went really wrong
				}
		}
		//add check for if signaled
	}
	if (ret == 0) {
		kill(pid);
		//setcode for timeout ?
	}
	//Read production of the child
	std::string	response(std::cin);
	//Error happened
	return (true);
}
