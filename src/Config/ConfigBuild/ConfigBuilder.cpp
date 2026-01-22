#include <cstdlib>
#include <stdexcept>
#include <sstream>

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

	_pushContext(MAIN);
    visit(*block);
	_popContext();

    return (_servers);
}


// Visit BLOCK or Directive
void ConfigBuilder::visit(const BlockNode& node)
{
	_validateStatement(node);

    const std::string& name = node.name;
	if (name == "server")
		_pushContext(SERVER);
	else if (name == "location")
		_pushContext(LOCATION);
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
		Location loc(_getCurrentCtxt());
		loc.setName(node.args[0]);
		// loc._isCGI();
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
	_direcSpecs["ast_root"]		= DirectiveSpecs(MAIN, 0, 0);
	_direcSpecs["server"]		= DirectiveSpecs(MAIN, 0, 0);
	_direcSpecs["location"]		= DirectiveSpecs(SERVER, 1, 1);

	_direcSpecs["listen"]		= DirectiveSpecs(SERVER, 1, 1);
	_direcSpecs["root"]			= DirectiveSpecs(SERVER|LOCATION, 1, 1);
	_direcSpecs["server_name"]	= DirectiveSpecs(SERVER, 1, 1);
	_direcSpecs["index"]		= DirectiveSpecs(SERVER|LOCATION, 1, 10);
	// _direcSpecs["autoindex"]		= DirectiveSpecs(SERVER|LOCATION, 0, 0);
}


// Handlers
void ConfigBuilder::_initHandlers()
{
	_handlers["listen"]		= &ConfigBuilder::_handleListen;
	_handlers["root"]		= &ConfigBuilder::_handleRoot;
	_handlers["server_name"]= &ConfigBuilder::_handleServerName;
	_handlers["index"]		= &ConfigBuilder::_handleIndex;
	// _handlers["autoindex"]	= &ConfigBuilder::_handleAutoIndex;
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


// Helpers
ConfigContext& ConfigBuilder::_getCurrentCtxt()
{
	return _contextStack.top();
}

void ConfigBuilder::_pushContext(ContextType type)
{
	ConfigContext newContext(type);
	if (_contextStack.size()) {
		newContext.inheritFrom(_getCurrentCtxt());
	}
	_contextStack.push(newContext);
}

void ConfigBuilder::_addServer(VirtualServer const& server)
{
		_servers.push_back(server);
}

void ConfigBuilder::_popContext()
{
	if (_contextStack.size()) {
		_contextStack.pop();
	}
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
