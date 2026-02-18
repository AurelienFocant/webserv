#include "cgi.hpp"

static bool	addFirstLineInfo(const RequestHandler& handler, std::vector<std::string>& vect) ;

bool	cgi::execute(const RequestHandler& handler, Connection& conn, char** env) {
	 //size must depend of number of argument and if an interpreter is needed ?
	char** argv = new char*[3];
	//Create argv for child exec
	argv[2] = NULL;
	try {
		argv[0] = convertStringToChar(findInterpreter(handler.getExtension()));
		argv[1] = convertStringToChar(handler.getResolvedPath()); //->maybe do one function that initialize whole argv based on file_to_execute
	}
	catch (std::exception& e) {
		//setup Response status code to Internal_server_error: here or up the stack
		std::cerr << "Fatal error; " << e.what() << std::endl;
		return (false); //Continue or crash the program ?
	}

	if (!launchCgi(conn, argv, env)) {
		//some errors happened, setup response code accordingly
		return (false);
	}

	if (fcntl(conn.cgi_fd[1], F_SETFL, O_NONBLOCK) < 0) {
		close(conn.cgi_fd[0]);
		close(conn.cgi_fd[1]);
		return (false);
	}
	if (fcntl(conn.cgi_fd[0], F_SETFL, O_NONBLOCK) < 0) {
		close(conn.cgi_fd[0]);
		close(conn.cgi_fd[1]);
		return (false);
	}
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
			if (!header.empty())
				vect.push_back(header);
			header.clear();
			if (it->first == "CONTENT_LENGTH" || it->first == "CONTENT_TYPE")
				header += it->first + "=" + it->second;
			else
				header += "HTTP_" + it->first + "=" + it->second;
		}
	}
	char **c_enc = new char*[vect.size()];
	for (size_t i = 0; i < vect.size(); ++i) {
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
	vect.push_back("SCRIPT_FILENAME=" + handler.getResolvedPath()); 
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


bool	cgi::launchCgi(Connection& conn, char** argv, char** env) {
//Pipe creation for communication with the child
	int	pipe_in[2];
	int	pipe_out[2];
	if (pipe(pipe_in) < 0) {
		return (false);
	}
	if (pipe(pipe_out) < 0) {
		return (false);
	}

//Creation of the subprocess
	pid_t	pid = fork();
	if (pid < 0)
		return (false);
	else if (pid == 0) { // Child process
	//Setup the pipe to write from the child
		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[1]);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[0]);
		 
		execve(argv[0], argv, env);
		
		close(pipe_in[0]);
		close(pipe_out[1]);
		delete[](env);
		delete[](argv);
		exit(EXIT_FAILURE);
	}
	else { // Parent process
	//Setup the pipe to listen in the parent
		//close(pipe_fd[0]);
	//	close(pipe_fd[1]);
		delete[](argv);
		delete[](env);
	}
	//Read production of the child
	close(pipe_in[0]);
	close(pipe_out[1]);

	conn.cgi_fd[0] = pipe_out[0];
	conn.cgi_fd[1] = pipe_in[1];
	conn.child_pid = pid;
	return (true);
}
