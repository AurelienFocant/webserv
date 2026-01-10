#ifndef CONFIGNODE_HPP
#define CONFIGNODE_HPP

#include <string>
#include <vector>
#include <iostream>


// SHOULD BE IT's own CPP file
struct ConfigNode {
	int			line;
	std::string	name;
	std::vector<std::string>	args;

	ConfigNode() : line(1), name(""), args() {}
	ConfigNode(const std::string& n) : line(1), name(n), args() {}
	virtual ~ConfigNode() {}
	virtual void print(int indent = 0) const = 0;

	protected:
	void indent(int n) const {
		for (int i = 0; i < n; ++i)
			std::cout << "  ";
	}
};

struct DirectiveNode : public ConfigNode {

	DirectiveNode(const std::string& n) : ConfigNode(n) {}

	void print(int indentLevel = 0) const {
		indent(indentLevel);
		std::cout << "Directive: " << name;
		for (size_t i = 0; i < args.size(); ++i)
			std::cout << " " << args[i];
		std::cout << std::endl;
	}
};

struct BlockNode : public ConfigNode {
	std::vector<ConfigNode*> children;

	BlockNode(const std::string& n) : ConfigNode(n) {}

	~BlockNode() {
		for (size_t i = 0; i < children.size(); ++i)
			delete children[i];
	}

	void print(int indentLevel = 0) const {
		indent(indentLevel);
		std::cout << "Block: " << name;
		for (size_t i = 0; i < args.size(); ++i)
			std::cout << " " << args[i];
		std::cout << std::endl;

		for (size_t i = 0; i < children.size(); ++i)
			children[i]->print(indentLevel + 1);
	}
};

#endif
