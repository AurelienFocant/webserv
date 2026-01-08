#pragma once

#include <map>
#include <stack>
#include <vector>
#include <stdexcept>
#include "VirtualServer.hpp"
#include "ConfigContext.hpp"
#include "ConfigNode.hpp"

class ConfigBuilder
{
	public:
		// MISSING constructors etc
		std::vector<VirtualServer> build(const ConfigNode *root);

		void visit(const BlockNode &node);
		void visit(const DirectiveNode &node);

		ConfigBuilder();
		~ConfigBuilder();

	private:
		bool	_has_root;

		std::stack<ConfigContext>	_contextStack;
		std::vector<VirtualServer>	_servers;


		/* Handlers */
		typedef void (ConfigBuilder::*DirectiveHandler)(const DirectiveNode &);
		std::map<std::string, DirectiveHandler>	_handlers;
		void _initHandlers();
		void _handleListen		(const DirectiveNode &);
		void _handleRoot   		(const DirectiveNode &);
		void _handleServerName	(const DirectiveNode &);

		/* Helpers */
		ConfigContext &_getCurrentCtxt();
		void _pushContext(ContextType type);
		void _popContext();
		void _error(int line, const std::string &);
};
