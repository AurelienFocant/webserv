#include <cstdlib>
#include <stdexcept>
#include <sstream>

#include "ConfigBuilder.hpp"
#include "DirectiveSpecs.hpp"

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

    return _servers;
}

// Visit BLOCK
void ConfigBuilder::visit(const BlockNode& node)
{

    const std::string& name = node.name;


	// set current BlockContext
	if (name == "server") {
		if (node.args.size() != 0)
			_error(node.line, std::string("Server block shouldn't have arguments"));
		_pushContext(SERVER);
	}
	else if (name == "location") {
		// check context in which we are !!
		if (node.args.size() != 1) {
			_error(node.line, std::string("Location block should have one argument"));
		}
		_pushContext(LOCATION);
		_getCurrentCtxt().setLocationName(node.args[0]);
	}
	else if (_contextStack.size() != 1) {
		_error(node.line, std::string("Unknown block: ") + name);
	}


	// visit all children of current BLOCK;
	_has_root = 0;
	for (size_t i = 0; i < node.children.size(); ++i) {
		// _visitChild(node);
		const ConfigNode* child = node.children[i];
		if (const BlockNode* bn = dynamic_cast<const BlockNode*>(child)) {
			visit(*bn);
		}
		else if (const DirectiveNode* dn = dynamic_cast<const DirectiveNode*>(child)) {
			visit(*dn);
		}
		else {
			_error(node.line, "Unknown node type in config tree");
		}
	}


	// manage finished block
	if (name == "server") {
		ConfigContext& ctx = _getCurrentCtxt();

		// shouldn't port be set to 80 by default ?
		// I think nginx does that but still complains if you dont put any listen
		// to be tested
		if (ctx.getPort() == -1)
			_error(node.line, "server missing listen directive");

		VirtualServer server;
		// should make a constructor taking a context as arg ??
		server.setPort(ctx.getPort());
		server.setRoot(ctx.getRoot());
		server.setServName(ctx.getServerName());
		server.setLocations(ctx.getLocations());
		server.setIndexes(ctx.getIndexes());
		server.setAutoindex(ctx.getAutoindex());

		_servers.push_back(server);
		_popContext();
	}
	else if (name == "location") {
		ConfigContext &locCtx = _getCurrentCtxt();

		Location loc;
		loc.setRoot(locCtx.getRoot());
		// loc.setIndexes(ctx.getIndexes());
		// loc.alias ???

		_popContext();
		_getCurrentCtxt().addLocation(locCtx.getLocationName(), loc);
	}
}

// Visit DIRECTIVE
void ConfigBuilder::visit(const DirectiveNode& node)
{
	_validateDirective(node);

	std::map<std::string, DirectiveHandler>::iterator it = _handlers.find(node.name);
	DirectiveHandler handler = it->second;
	(this->*handler)(node);
}

void	ConfigBuilder::_validateDirective(DirectiveNode const& node)
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
	_direcSpecs["listen"]		= DirectiveSpecs(SERVER, 1, 1);
	_direcSpecs["root"]			= DirectiveSpecs(SERVER|LOCATION, 1, 1);
	_direcSpecs["server_name"]	= DirectiveSpecs(SERVER, 1, 1);
	_direcSpecs["index"]		= DirectiveSpecs(SERVER|LOCATION, 1, 10);
	// _direcSpecs["autoindex"]		= DirectiveSpecs(SERVER|LOCATION, 0, 0);
}

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

ConfigBuilder::ConfigBuilder()
	: _has_root(0)
{
}

ConfigBuilder::~ConfigBuilder()
{
}
