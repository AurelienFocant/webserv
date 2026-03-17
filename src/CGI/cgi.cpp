#include "cgi.hpp"

static bool	addFirstLineInfo(const RequestHandler& handler, std::vector<std::string>& vect);

void	deleteEnv(char **env)
{
	char **ptr = env;
	while (*env) {
		delete[](*env);
		env++;
	}
	delete[](ptr);

}

bool	cgi::execute(const RequestHandler& handler, Connection& conn, char** env)
{
	char* argv[3];

	if (!handler.getCGIExec().empty())
	{
		argv[0] = strdup(handler.getCGIExec().c_str());
		argv[1] = strdup(handler.getResolvedPath().c_str());
		argv[2] = NULL;
	}
	else
	{
		argv[0] = strdup(handler.getResolvedPath().c_str());
		argv[1] = strdup(handler.getResolvedPath().c_str());
		argv[2] = NULL;
	}

	if (!argv[0] || !argv[1])
	{
		deleteEnv(env);
		return (false);
	}

	if (!launchCgi(conn, argv, env)) {
		deleteEnv(env);
		return (false);
	}

	free(argv[0]);
	argv[0] = NULL;
	free(argv[1]);
	argv[1] = NULL;

	deleteEnv(env);

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

char**	cgi::buildCgiEnv(const RequestHandler& handler)
{
	std::multimap<std::string, std::string>	cpp_env = handler.getRequest().getHeaders();
	std::string	previous_key = "";
	std::string	header;
	std::vector<std::string>	vect;
	addFirstLineInfo(handler, vect);
	for (std::multimap<std::string, std::string>::const_iterator it = cpp_env.begin(); it != cpp_env.end(); ++it) {
		if (previous_key == it->first) {
			vect.back() += ", " + it->second;
		}
		else {
			previous_key.clear();
			previous_key = it->first;
			header.clear();
			if (it->first == "CONTENT_LENGTH" || it->first == "CONTENT_TYPE")
				header += it->first + "=" + it->second;
			else
				header += "HTTP_" + it->first + "=" + it->second;
			vect.push_back(header);
		}
	}
	if (handler.getRequest().getHeaderValues("CONTENT_LENGTH").at(0) == "") {
		std::stringstream	stream;
		stream << handler.getRequest().getContentLength();
		header.clear();
		header += "CONTENT_LENGTH=" + stream.str();
		vect.push_back(header);
	}


	std::stringstream ss;
	ss << handler.getResponse().getCounterSession();
	vect.push_back("SESSION_VISITS=" + ss.str());


	char **c_enc = new char*[vect.size() + 1];
	c_enc[vect.size()] = NULL;
	for (size_t i = 0; i < vect.size(); ++i) {
		c_enc[i] = new char[vect.at(i).size() + 1];
		std::strcpy(c_enc[i], vect.at(i).c_str());
	}
	return (c_enc);
}

static bool	addFirstLineInfo(const RequestHandler& handler, std::vector<std::string>& vect)
{
	vect.push_back("REQUEST_METHOD=" + methodToString(handler.getRequest().getMethod()));
	vect.push_back("SERVER_PROTOCOL=" + handler.getRequest().getHttpVersion());
	vect.push_back("QUERY_STRING=" + handler.getQuery());

	if (!handler.getCGIExec().empty())
	{
		vect.push_back("SCRIPT_NAME=" + handler.getScriptName());
		vect.push_back("SCRIPT_FILENAME=" + handler.getCGIExec());
		vect.push_back("PATH_INFO=" + handler.getPathInfo());
		vect.push_back("PATH_TRANSLATED=" + handler.getCtx().root + handler.getPathInfo());
	}
	else
	{
		vect.push_back("SCRIPT_NAME=" + handler.getScriptName());
		vect.push_back("SCRIPT_FILENAME=" + handler.getResolvedPath());
		if (!handler.getPathInfo().empty())
		{
			vect.push_back("PATH_INFO=" + handler.getPathInfo());
			vect.push_back("PATH_TRANSLATED=" + handler.getCtx().root + handler.getPathInfo());
		}
	}
	return (true);
}

bool	cgi::launchCgi(Connection& conn, char** argv, char** env)
{
	//Pipe creation for communication with the child
	int	pipe_in[2];
	int	pipe_out[2];
	if (pipe(pipe_in) < 0) {
		return (false);	
	}
	if (pipe(pipe_out) < 0) {
		close(pipe_in[0]);
		close(pipe_in[1]);
		return (false);
	}

	//Creation of the subprocess
	pid_t	pid = fork();
	if (pid < 0)
		return (false);
	else if (pid == 0) {
		// Child process
/* 		std::string script_path(argv[0]);
		size_t last_slash = script_path.rfind('/');
		if (last_slash != std::string::npos)
		{
			std::string dir = script_path.substr(0, last_slash);
			std::string	filename = script_path.substr(last_slash + 1);
			chdir(dir.c_str());
			free(argv[0]);
			argv[0] = strdup(("./" + filename).c_str());
		} */

		//Setup the pipe to write from the child
		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[1]);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[0]);

		close(pipe_in[0]);
		close(pipe_out[1]);

		execve(argv[0], argv, env);

		//In case of error in the child, clean everything and exit
		delete[](env);
		free(argv[0]);
		argv[0] = NULL;
		free(argv[1]);
		argv[1] = NULL;

		std::cerr << errno << std::endl;
		perror("EXECVE FAILED");
		exit(EXIT_FAILURE);
	}

	// Parent process
	//Setup the pipe to listen in the parent
	close(pipe_in[0]);
	close(pipe_out[1]);

	//Pass in pipe writing head, pipe_in[1], and out_pipe reading head, pipe_out[0], to connection.
	//Also pass child pid for further child management
	conn.cgi_fd[0] = pipe_out[0];
	conn.cgi_fd[1] = pipe_in[1];
	conn.child_pid = pid;
	conn.cgi_timeout = std::time(NULL);
	return (true);
}

bool	cgi::parseOutput(Response& response)
{
	size_t	end = response.getBody().find("\r\n\r\n");
	int 	separator_len = 4;

	if (end == std::string::npos) {
		end = response.getBody().find("\n\n");
		separator_len = 2;
	}
	
	if (end == std::string::npos)
		return false;

	std::string	headers_str = response.getBody().substr(0, end);
	size_t 		body_start = end + separator_len;
	std::string	body = response.getBody().substr(body_start);

	std::stringstream ss(headers_str);
	std::string line;
	while (std::getline(ss, line)) {
		if (line.empty() || line == "\r")
			continue;
		
		if (!line.empty() && line[line.size() - 1] == '\r')
			line = line.substr(0, line.size() - 1);
		
		size_t colon = line.find(":");
		if (colon == std::string::npos)
			continue;
		
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		if (key == "Status") {
			size_t space = value.find(' ');
			std::string code_str = (space != std::string::npos) ? value.substr(0, space) : value;
			int status_code = atoi(code_str.c_str());
			response.setStatusCode(status_code);
		}
		else {
			response.setHeader(key, value);
		}
	}
	response.setBody(body);
	return true;
}
