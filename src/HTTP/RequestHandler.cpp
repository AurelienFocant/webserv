
#include "RequestHandler.hpp"
#include "VirtualServer.hpp"
#include "pathResolver.hpp"
#include "methods.hpp"

/* ////////////REQUEST HANDLER////////////////// */

RequestHandler::RequestHandler() 
	: _server(NULL)
	, _ctx()
	, _request()
	, _response()
{}

RequestHandler::~RequestHandler() {}

void	RequestHandler::processRequest(const std::string& input)
{
		_request.addInput(input);
		_request.parseRequest();
		return ;
}

void	RequestHandler::processBody()
{
	_request.handleBody((*_ctx.matched_location).getMaxBodySize());
}

void	RequestHandler::handleRequest()
{
	/* DEBUG */
	std::cout << "[resolvePath] Request path: " << _ctx.request_path << std::endl;

	std::cout << "[resolvePath] LOCATION NAME: " << _ctx.matched_location->getName() << std::endl;
	const std::set<std::string>& methods = _ctx.matched_location->getAllowedMethods();
	for (std::set<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it)
		std::cerr << "[resolvePath] ALLOWED: " << *it << std::endl;
	/* //////////////////////////// */
	_response.setMethod(_request.getMethod());
	if (_request.getStatusCode() != OK)
		_response.setStatusCode(_request.getStatusCode());

	else if (path::resolve(_ctx, _response) && !_ctx.is_cgi)
			method::dispatch(_ctx, _request, _response);

	else if (_ctx.is_cgi)
	{
		_response.isCGI = true;
		return;
	}

	resp::prepareResponse(_response, _request, _server->getErrorPages());
}

void	RequestHandler::findLocation()
{
	if (_ctx.request_path.empty())
		path::extract(_ctx, _request, _response);
	path::matchLocation(_ctx, _server);
}

bool	RequestHandler::isAllowedMethod()
{
	return (method::isAllowed(_ctx, _request, _response));
}

void	RequestHandler::requestIsComplete()
{
	_request.setComplete(true);
}

void	RequestHandler::resetPathContext()
{
	_ctx = PathContext();
}

void	RequestHandler::clean()
{
	_request.cleanRequest();
	_response.cleanResponse();
	_server = NULL;
	_ctx = PathContext();
}

/* Getters */
const Request&	RequestHandler::getRequest() const
{
	return (_request);
}

const Response&	RequestHandler::getResponse() const
{
	return (_response);
}

const VirtualServer*	RequestHandler::getVirtualServer() const
{
	return _server;
}

std::string	RequestHandler::getQuery() const
{
	return (_ctx.query);
}

std::string	RequestHandler::getPathInfo() const 
{
	return (_ctx.path_info);
}

std::string	RequestHandler::getScriptName() const 
{
	return (_ctx.script_name);
}

t_extension	RequestHandler::getExtension() const	
{
	return (_ctx.matched_extension);
}

std::string	RequestHandler::getResolvedPath() const	
{
	return (_ctx.resolved_path);
}


std::string	RequestHandler::getCGIExec() const
{
	return _ctx.cgi_exec;
}


/* Setters */
void	RequestHandler::setVirtualServer(const VirtualServer& server)
{
	_server = &server;
}

void	RequestHandler::setRoot(const std::string& root)
{
	_ctx.root = root;
}
