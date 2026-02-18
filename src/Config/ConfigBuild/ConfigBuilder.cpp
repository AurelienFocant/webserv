#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <set>

#include "ConfigBuilder.hpp"
#include "DirectiveSpecs.hpp"

// Entrypoint
std::vector<VirtualServer> ConfigBuilder::build(const ConfigNode* root)
{
    const BlockNode* block =
        dynamic_cast<const BlockNode*>(root);
    if (!block)
        _error(0, "Config root must be a block");

    _initHandlers();
	_initDirectiveSpecs();

	_pushNewInheritedCtxt(MAIN);
    visit(*block);
	_popContext();

    return (_servers);
}


// Visit BLOCK or DIRECTIVE
void ConfigBuilder::visit(const BlockNode& node)
{
	_validateStatement(node);

    const std::string& name = node.name;
	if (name == "server")
		_pushNewInheritedCtxt(SERVER);
	else if (name == "location")
		_pushNewInheritedCtxt(LOCATION);
	else if (name != "ast_root")
		_error(node.line, std::string("Unknown block: ") + name);


	_has_root = 0;
	for (size_t i = 0; i < node.children.size(); ++i)
		_visitChild((node.children[i]));


	if (name == "server") {
		VirtualServer server(_getCurrentCtxt());
		_popContext();
		_addServer(server);
	}
	else if (name == "location") {
		std::string location_name = node.args[0];
		Location loc(_getCurrentCtxt());
		loc.setName(location_name);
		_popContext();
		_getCurrentCtxt().addLocation(loc.getName(), loc);
	}
}

void ConfigBuilder::visit(const DirectiveNode& node)
{
	_validateStatement(node);

	std::map<std::string, DirectiveHandler>::iterator it = _handlers.find(node.name);
	DirectiveHandler handler = it->second;
	(this->*handler)(node);
}

void	ConfigBuilder::_visitChild(ConfigNode const* child) 
{
		if (const BlockNode* bn = dynamic_cast<const BlockNode*>(child))
			visit(*bn);
		else if (const DirectiveNode* dn = dynamic_cast<const DirectiveNode*>(child))
			visit(*dn);
		else
			_error(child->line, "Unknown node type in config tree");
}


// Validation
void	ConfigBuilder::_validateStatement(ConfigNode const& node)
{
	std::map<std::string, DirectiveSpecs>::iterator it = _direcSpecs.find(node.name);
	if (it == _direcSpecs.end())
		_error(node.line ,std::string("Unknown directive: ") + node.name);

	DirectiveSpecs spec = it->second;
	if (!(_getCurrentCtxt().getType() & spec.allowedCtxts))
		_error(node.line, std::string("Directive ") + node.name + std::string(" forbidden in this context"));

	if (node.args.size() < (size_t) spec.min_args)
		_error(node.line, std::string("Directive ") + node.name + std::string(" doesn't have enough arguments"));

	if (node.args.size() > (size_t) spec.max_args)
		_error(node.line, std::string("Directive ") + node.name + std::string(" has too many arguments"));
}

void ConfigBuilder::_initDirectiveSpecs()
{
	_direcSpecs["ast_root"]				= DirectiveSpecs(MAIN, 0, 0);
	_direcSpecs["server"]	   			= DirectiveSpecs(MAIN, 0, 0);
	_direcSpecs["location"]	   			= DirectiveSpecs(SERVER, 1, 1);

	_direcSpecs["listen"]	   			= DirectiveSpecs(SERVER, 1, 1);
	_direcSpecs["root"]		   			= DirectiveSpecs(SERVER|LOCATION, 1, 1);
	_direcSpecs["server_name"] 			= DirectiveSpecs(SERVER, 1, 1);
	_direcSpecs["index"]	   			= DirectiveSpecs(SERVER|LOCATION, 1, 10);
	_direcSpecs["autoindex"]   			= DirectiveSpecs(SERVER|LOCATION, 1, 1);
	_direcSpecs["keepalive_time"]		= DirectiveSpecs(SERVER|LOCATION, 1, 1);
	_direcSpecs["keepalive_timeout"]	= DirectiveSpecs(SERVER|LOCATION, 1, 1);
	_direcSpecs["return"]				= DirectiveSpecs(SERVER|LOCATION, 2, 2);
	_direcSpecs["allowed_methods"]		= DirectiveSpecs(SERVER|LOCATION, 1, 3);
	_direcSpecs["cgi"]					= DirectiveSpecs(LOCATION, 1, 1);
	_direcSpecs["virtual"]				= DirectiveSpecs(LOCATION, 1, 1);
	_direcSpecs["alias"]				= DirectiveSpecs(SERVER|LOCATION, 1, 1);
	_direcSpecs["error_page"]			= DirectiveSpecs(SERVER|LOCATION, 2, 999);
}


// Handlers
void ConfigBuilder::_initHandlers()
{
	_handlers["listen"]				= &ConfigBuilder::_handleListen;
	_handlers["root"]				= &ConfigBuilder::_handleRoot;
	_handlers["server_name"]		= &ConfigBuilder::_handleServerName;
	_handlers["index"]				= &ConfigBuilder::_handleIndex;
	_handlers["autoindex"]			= &ConfigBuilder::_handleAutoindex;
	_handlers["keepalive_time"]		= &ConfigBuilder::_handleKeepaliveTime;
	_handlers["keepalive_timeout"]	= &ConfigBuilder::_handleKeepaliveTimeout;
	_handlers["return"]				= &ConfigBuilder::_handleReturn;
	_handlers["allowed_methods"]	= &ConfigBuilder::_handleAllowedMethods;
	_handlers["cgi"]				= &ConfigBuilder::_handleCGI;
	_handlers["virtual"]			= &ConfigBuilder::_handleVirtualLocation;
	_handlers["alias"]				= &ConfigBuilder::_handleAlias;
	_handlers["error_page"]			= &ConfigBuilder::_handleErrorPages;
}

void ConfigBuilder::_handleListen(const DirectiveNode& d)
{
	std::stringstream ss(d.args[0]);
	int port;
	char c;

	ss >> port;
	if (ss.fail() || (ss >> c))
		_error(d.line, "invalid port format");
	if (port <= 0 || port > 65535)
		_error(d.line, "invalid port");

	_getCurrentCtxt().setPort(port);
}

void ConfigBuilder::_handleRoot(const DirectiveNode& d)
{
	if (_has_root)
		_error(d.line, "root directive is a duplicate");

	_getCurrentCtxt().setRoot(d.args[0]);
	_has_root = 1;
}

void ConfigBuilder::_handleIndex(const DirectiveNode& d)
{
	_getCurrentCtxt().setIndexes(d.args);
}

void ConfigBuilder::_handleServerName(const DirectiveNode& d)
{
	_getCurrentCtxt().setServerName(d.args[0]);
}

void ConfigBuilder::_handleAutoindex(const DirectiveNode& d)
{
	if (d.args[0] == "on")
		_getCurrentCtxt().setAutoindex(true);
	else if (d.args[0] == "off")
		_getCurrentCtxt().setAutoindex(false);
	else
		_error(d.line, "unknown option for 'autoindex' directive");
}

void ConfigBuilder::_handleKeepaliveTime(const DirectiveNode& d)
{
	std::stringstream ss(d.args[0]);
	int time;
	char c;

	ss >> time;
	if (ss.fail() || (ss >> c))
		_error(d.line, "invalid time format");
	if (time < 60 || time > 3600)
		_error(d.line, "Keepalive_time should be between 1min and 1h");

	_getCurrentCtxt().setKeepalive_time(time);
}

void ConfigBuilder::_handleKeepaliveTimeout(const DirectiveNode& d)
{
	std::stringstream ss(d.args[0]);
	int time;
	char c;

	ss >> time;
	if (ss.fail() || (ss >> c))
		_error(d.line, "invalid time format");
	if (time < 1 || time > 600)
		_error(d.line, "Keepalive_timeout should be between 1sec and 10min");

	_getCurrentCtxt().setKeepalive_timeout(time);
}

void ConfigBuilder::_handleReturn(const DirectiveNode& d)
{
	std::stringstream ss(d.args[0]);
	int code;
	char c;

	ss >> code;
	if (ss.fail() || (ss >> c))
		_error(d.line, "invalid status code format");
	if (code < 100 || code > 505)
		_error(d.line, "invalid status code");
	_getCurrentCtxt().setRedirectCode(code);


	std::string	s(d.args[1]);
	_getCurrentCtxt().setRedirect(s);
}

void ConfigBuilder::_handleAllowedMethods(const DirectiveNode& d)
{
	std::set<std::string>	allowed_methods;
	for (size_t i = 0; i < d.args.size(); i++) {
		std::string method = d.args[i];

		if (allowed_methods.find(method) != allowed_methods.end())
			_error(d.line, "duplicate method");

		if (method == "GET" || method == "POST" || method == "DELETE")
			allowed_methods.insert(method);
		else
			_error(d.line, "invalid method!");
	}

	_getCurrentCtxt().setAllowedMethods(allowed_methods);
}

void ConfigBuilder::_handleCGI(const DirectiveNode& d)
{
	std::string arg = d.args[0];

	if (arg == "off") {
		_getCurrentCtxt().setCGI(false); return;
	}
	if (arg == "on") {
		_getCurrentCtxt().setCGI(true); return;
	}
	_error(d.line, "only on of off values after 'cgi' directive");
}

void ConfigBuilder::_handleVirtualLocation(const DirectiveNode& d)
{
	std::string arg = d.args[0];

	if (arg == "off") {
		_getCurrentCtxt().setVirtualLocation(false); return;
	}
	if (arg == "on") {
		_getCurrentCtxt().setVirtualLocation(true); return;
	}
	_error(d.line, "only on of off values after 'virtual' directive");
}

void ConfigBuilder::_handleAlias(const DirectiveNode& d)
{
	_getCurrentCtxt().setAlias(d.args[0]);
}

void ConfigBuilder::_handleErrorPages(const DirectiveNode& d)
{
	std::vector<std::string>::const_iterator it;

	for (it = d.args.begin(); it != d.args.end() - 1; ++it) {
		std::stringstream	ss(*it);
		int		n;
		char	c;
		if (ss >> n && ss >> c)
			_error(d.line, "Only numerical codes allowed after error_page directive");

		_getCurrentCtxt().setErrorPage(n, *(d.args.end()));
	}
}



// Utils
ConfigContext& ConfigBuilder::_getCurrentCtxt()
{
	return _contextStack.top();
}

void ConfigBuilder::_pushNewInheritedCtxt(ContextType type)
{
	ConfigContext newContext(type);

	if (_contextStack.size()) {
		newContext.inheritFrom(_getCurrentCtxt());
	}
	_contextStack.push(newContext);
}

void ConfigBuilder::_popContext()
{
	if (_contextStack.size()) {
		_contextStack.pop();
	}
}

void ConfigBuilder::_addServer(VirtualServer const& server)
{
		_servers.push_back(server);
}

void ConfigBuilder::_error(int line, const std::string& msg)
{
	std::ostringstream oss;
	oss << "Config error at line " << line << ": " << msg;
	throw std::runtime_error(oss.str());
}


// Constructors
ConfigBuilder::ConfigBuilder()
	: _has_root(0)
{
}

ConfigBuilder::~ConfigBuilder()
{
}
