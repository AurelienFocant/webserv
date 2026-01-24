#include "ConfigParser.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>

ConfigParser::ConfigParser(const std::vector<t_Token>& tokens)
	: _tokens(tokens), _pos(0)
{
}

ConfigParser::~ConfigParser()
{
	delete (_ast_root);
}

t_Token ConfigParser::_peek() const
{
	return _tokens[_pos];
}

t_Token ConfigParser::_advance()
{
	return _tokens[_pos++];
}

bool ConfigParser::_atEnd() const
{
	return _pos >= _tokens.size();
}

void ConfigParser::_expect(e_tokenType type)
{
	if (_peek().tkType != type) {
		std::ostringstream ss;
		ss << _peek().line;
		std::cerr << "Syntax error on line " << ss.str() << '\n';
		throw (std::runtime_error("Syntax error"));
	}

	if (_atEnd()) {
		std::cerr << "Unexpected end of file\n";
		throw (std::runtime_error("Syntax error"));
	}
}

void ConfigParser::parseConfig()
{
	_ast_root = new BlockNode("ast_root");
	while (!_atEnd())
		_ast_root->children.push_back(_parseStatement());
}

BlockNode*	ConfigParser::getRoot(void)
{
	return (_ast_root);
}

ConfigNode* ConfigParser::_parseStatement()
{
	_expect(WORD);
	t_Token first = _advance();

	size_t look = _pos;
	while (look < _tokens.size() && _tokens[look].tkType == WORD)
		look++;

	if (look < _tokens.size() && _tokens[look].tkType == LBRACE) {
		_pos--;
		return _parseBlock();
	}
	return _parseDirective(first);
}

BlockNode* ConfigParser::_parseBlock()
{
	t_Token name = _advance();
	BlockNode* block = new BlockNode(name.lexeme);

	block->line = name.line;
	while (!_atEnd() && _peek().tkType == WORD)
		block->args.push_back(_advance().lexeme);

	_expect(LBRACE);
	_advance();

	while (!_atEnd() && _peek().tkType != RBRACE)
		block->children.push_back(_parseStatement());

	_expect(RBRACE);
	_advance();

	return block;
}

DirectiveNode* ConfigParser::_parseDirective(const t_Token& first)
{
	DirectiveNode* dir = new DirectiveNode(first.lexeme);

	// first or name ? Dir or block ??
	dir->line = first.line;
	while (!_atEnd() && _peek().tkType == WORD)
		dir->args.push_back(_advance().lexeme);

	_expect(SEMICOLON);
	_advance();

	return dir;
}
