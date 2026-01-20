#pragma once

#include <map>
#include <stack>
#include <vector>

#include "VirtualServer.hpp"
#include "ConfigContext.hpp"
#include "ConfigNode.hpp"
#include "DirectiveSpecs.hpp"

class ConfigBuilder
{
	public:
		std::vector<VirtualServer> build(const ConfigNode *root);

		void visit(const BlockNode &node);
		void visit(const DirectiveNode &node);

		// MISSING constructors etc
		ConfigBuilder();
		~ConfigBuilder();

	private:
		std::vector<VirtualServer>	_servers;

		void	_initDirectiveSpecs(void);
		std::map<std::string, DirectiveSpecs>	_direcSpecs;
		void	_validateStatement(ConfigNode const& node);

		typedef void (ConfigBuilder::*DirectiveHandler)(const DirectiveNode &);
		std::map<std::string, DirectiveHandler>	_handlers;
		void _initHandlers();
		void _handleListen		(const DirectiveNode& d);
		void _handleRoot   		(const DirectiveNode& d);
		void _handleServerName	(const DirectiveNode& d);
		void _handleIndex		(const DirectiveNode& d);

		std::stack<ConfigContext>	_contextStack;
		ConfigContext&				_getCurrentCtxt(void);
		void _pushContext(ContextType type);
		void _popContext (void);
		void _error(int line, const std::string &);

		void	_visitChild(ConfigNode const* node);
		bool	_has_root;
};
