#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <vector>
#include "ConfigTokenizer.hpp"
#include "ConfigNode.hpp"

class ConfigParser {
private:
    std::vector<t_Token> _tokens;
    size_t _pos;

    t_Token	_peek() const;
    t_Token	_advance();
    bool	_atEnd() const;
    void	_expect(e_tokenType type);

	BlockNode*	_ast_root;
    ConfigNode*		_parseStatement();
    BlockNode* 		_parseBlock();
    DirectiveNode*	_parseDirective(const t_Token& first);

public:
	// missing constructors etc
    ConfigParser(const std::vector<t_Token>& tokens);
	~ConfigParser();

    void parseConfig();

	BlockNode* getRoot(void);
};

#endif
