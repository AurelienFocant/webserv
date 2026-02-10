#include "cgi.hpp"

static bool	addFirstLineInfo(const RequestHandler& handler, std::vector<std::string>& vect) ;

bool	cgi::execute(const RequestHandler& handler, Response& response, char** env) {
	//For compilation errors
		(void)response;
	 //size must depend of number of argument and if an interpreter is needed ?
	char** argv = new char*[2];
	//Create argv for child exec
	try {
		argv[0] = findInterpreter(handler.getExtansion());
		argv[1] = convertStringToChar(handler.getScriptName()); //->maybe do one function that initialize whole argv based on file_to_execute
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
	delete[](env);
	return (true);
}

char**	cgi::buildCgiEnv(const RequestHandler& handler) {
	std::multimap<std::string, std::string>	cpp_env = handler.getRequest().getHeaders();
	std::string	previous_key = "";
	std::string	header;
	std::vector<std::string>	vect;
	addFirstLineInfo(handler, vect);
	for (std::multimap<std::string, std::string>::const_iterator it = cpp_env.begin(); it != cpp_env.end(); ++it) {
		if (previous_key == it->first) {
			header += ", " + it->second;
		}
		else {
			previous_key.clear();
			previous_key = it->first;
			vect.push_back(header);
			header.clear();
			if (it->first == "CONTEN_LENGTH" || it->first == "CONTENT_TYPE")
				header += it->first + "=" + it->second;
			else
				header += "HTTP_" + it->first + "=" + it->second;
		}
	}
	size_t	i = 0;
	char **c_enc = new char*[vect.size()];
	for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); ++it) {
		c_enc[i] = new char[vect.at(i).size() + 1];
		std::strcpy(c_enc[i], vect.at(i).c_str());
	}
	return (c_enc);
}

static bool	addFirstLineInfo(const RequestHandler& handler, std::vector<std::string>& vect) {
	vect.push_back("REQUEST_METHOD=" + methodToString(handler.getRequest().getMethod()));
	vect.push_back("SERVER_PROTOCOL=" + handler.getRequest().getRequestUri());
	vect.push_back("QUERY_STRING=" + handler.getQuery());
	vect.push_back("SCRIPT_NAME=" + handler.getScriptName());
	return (true);
}

char*	cgi::findInterpreter(const t_extension& extension) {
	switch (extension) {
		case (PY):
			return (convertStringToChar("/usr/bin/python3")); //--> potential problems, need decisions on that 
		case (SH):
			return (convertStringToChar("/usr/bin/bash"));
		default:
			return (NULL);
	}
}

char*	cgi::convertStringToChar(const std::string& string) {
	char* str = new char[string.size()];
	str[string.size()] = '\0';
	for (size_t i = 0; i < string.size(); ++i) {
		str[i] = string[i];
	}
	return (str);
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

		execve(argv[0], argv, env);
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
	int	ret;
	while (time > 0) {
		ret = waitpid(pid, &status, WNOHANG);
		switch (ret) {
			case (-1):
				switch (errno) {
					case (ECHILD):
						break ;
					case (EINTR):
						break ;
					case (EINVAL):
						break ;
					default:
						(void)pid;
						//SOMETHING went really wrong
				}
				break ;
			case (0):
				time -= 50;
				if (usleep(50) < 0) {
					kill(pid, SIGINT);
					//setStatus code internal error
					return (false);
				}
				break ;
			default:
				if (ret == pid)
					//Do SOMething
				(void)pid;
		}
	}
	//add check for if signaled
	if (ret == 0) {
		kill(pid, SIGINT);
		//setcode for timeout ?
	}
	//Read production of the child
	std::string	response;
	std::cin >> response;
	//Error happened
	return (true);
}
