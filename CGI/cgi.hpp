
#ifndef		CGI_HPP
# define	CGI_HPP

# include <unistd.h>

namespace	cgi {
		bool	execute() ;
		bool	launchCgi(/*name of excec as parameter ?*/);
		char	*findInterpreter(const std::string& extension) ;
		char	**buildCgiEnv(const Request& request);
};

#endif


# include "Cgi.hpp"

bool	cgi::execute(const requestHandler& handler, Response& response, char **env) {
	char** argv;	
	//Create argv for child exec
	try {
		char	argv[2]; //size must depend of number of argument and if an interpreter is needed
		argv[0] = findInterpreter(handler.extension);
		argv[1] = file_to_execute.c_str(); //->maybe do one function that initialize whole argv based on file_to_execute
	}
	catch (std::exception& e) {
		//setup Response status code to Internal_server_error: here or up the stack
		std::cerr << "Fatal error; " << e.what() << std::endl;
		return (false); //Continue or crash the program ?
	}

	if (!launchCgi(argv, env)) {
		//some errors happened, setup response code accordingly
	}
	delete[](argv);
	//delete[](cgi_env); -> probably needed, depend on CppEnv implementation
	return (true);
}

char	cgi::**buildCgiEnv(const Request& request) {
	std::multimap<std::string, std::string>	cpp_env = request.getHeaders;
	std::string	previous_key = "";
	std::string	header;
	std::vector<std::string>	vect;
	for (std::multimap<std::string, std::string>::const_iterator it = cpp_env.begin(); it != cpp_env.end(); ++it) {
		if (previous_key == it->first) {
			header += ", " + it->second;
		}
		else {
			previous_key.clear();
			previous_key = it->first;
			vect.insert(0, header);
			header.clear();
			header += "HTTP_" + it->first + "=" + it->second;
		}
	}
	size_t	i = 0;
	char **c_enc = new[](sizeof(char*) * vect.size());
	for (std::vector<std::string> it = vect.begin(); it != vect.end(); ++it) {
		c_enc.[i] = vect.at(i).c_str();
	}
	return (c_enc);
}


char	cgi::*findInterpreter(const std::string& extension) {
	char	*response;
	if (extension == ".py")
		response = "/*path to python interpreter*/";
	else if (extension == ".sh")
		response = "/*path to shell interpreter*/";
	else
		response = NULL;
	return (response);
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
		delete[](env);
		exit(EXIT_FAILURE);
	}
	else { // Parent process
	//Setup the pipe to listen in the parent
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDIN_FILENO);
		close(pipe_fd[1]);
		delete[](env);
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
