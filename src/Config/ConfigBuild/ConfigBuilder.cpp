#include "ConfigBuilder.hpp"
#include <cstdlib>
#include <stdexcept>
#include <sstream>

std::vector<VirtualServer> ConfigBuilder::build(const ConfigNode* root)
{
    const BlockNode* block =
        dynamic_cast<const BlockNode*>(root);
    if (!block)
        _error(0, "Config root must be a block");

    _initHandlers();
    _contextStack.push(ConfigContext(MAIN));

    visit(*block);

    _contextStack.pop();
    return _servers;
}

void ConfigBuilder::visit(const BlockNode& node)
{
    const std::string& name = node.name;

    if (name == "server") {
        if (node.args.size() != 0) {
			_error(node.line, std::string("Server block shouldn't have arguments"));
		}
        _pushContext(SERVER);
    }
	else if (name == "location") {
        if (node.args.size() != 1) {
			_error(node.line, std::string("Location block should have one argument"));
        }
        _pushContext(LOCATION);
		_getCurrentCtxt().setLocationName(node.args[0]);
    }
	else if (_contextStack.size() != 1) {
        _error(node.line, std::string("Unknown block: ") + name);
    }

    for (size_t i = 0; i < node.children.size(); ++i) {
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

    if (name == "server") {
        ConfigContext& ctx = _getCurrentCtxt();

        if (ctx.getPort() == -1)
            _error(node.line, "server missing listen directive");

        VirtualServer server;
        server.setPort(ctx.getPort());
        server.setRoot(ctx.getRoot());
        server.setServName(ctx.getServerName());
        server.setLocations(ctx.getLocations());

        _servers.push_back(server);
        _popContext();
    }
	else if (name == "location") {
        ConfigContext &locCtx = _getCurrentCtxt();

        Location loc;
        loc.root = locCtx.getRoot();
		// loc.alias = locCtx.getAlias ???
        
        _popContext();
        _getCurrentCtxt().addLocation(locCtx.getLocationName(), loc);
    }
}

void ConfigBuilder::visit(const DirectiveNode& node)
{
    const std::string& name = node.name;
    std::map<std::string, DirectiveHandler>::iterator it = _handlers.find(name);

    if (it != _handlers.end()) {
        DirectiveHandler handler = it->second;
        (this->*handler)(node);
    } else {
        _error(node.line, std::string("Unknown directive: ") + name);
    }
}

void ConfigBuilder::_initHandlers()
{
    _handlers["listen"] = &ConfigBuilder::_handleListen;
    _handlers["root"] = &ConfigBuilder::_handleRoot;
    _handlers["server_name"] = &ConfigBuilder::_handleServerName;
}

void ConfigBuilder::_handleListen(const DirectiveNode& d)
{
    if (_getCurrentCtxt().getType() != SERVER)
        _error(d.line, "listen only allowed in server");

    if (d.args.size() != 1)
        _error(d.line, "listen expects 1 argument");

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
    if (_getCurrentCtxt().getType() == MAIN)
        _error(d.line, "root not allowed in main context");

    if (d.args.size() != 1)
        _error(d.line, "root expects 1 argument");

    _getCurrentCtxt().setRoot(d.args[0]);
}

void ConfigBuilder::_handleServerName(const DirectiveNode& d)
{
    if (_getCurrentCtxt().getType() != SERVER)
        _error(d.line, "server_name only allowed in server");

    if (d.args.size() != 1)
        _error(d.line, "server_name expects 1 argument");

    _getCurrentCtxt().setServerName(d.args[0]);
}

ConfigContext& ConfigBuilder::_getCurrentCtxt()
{
    return _contextStack.top();
}

void ConfigBuilder::_pushContext(ContextType type)
{
    ConfigContext newContext(type);
    newContext.inheritFrom(_getCurrentCtxt());
    _contextStack.push(newContext);
}

void ConfigBuilder::_popContext()
{
    _contextStack.pop();
}

void ConfigBuilder::_error(int line, const std::string& msg)
{
    std::ostringstream oss;
    oss << "Config error at line " << line << ": " << msg;
    throw std::runtime_error(oss.str());
}
