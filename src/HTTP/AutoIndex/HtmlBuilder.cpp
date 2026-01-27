#include "HtmlBuilder.hpp"

HtmlBuilder::HtmlBuilder()
	: _html("")
{
}

HtmlBuilder::HtmlBuilder(const std::string& text)
	: _html(text)
{
}

HtmlBuilder::HtmlBuilder(const HtmlBuilder& other)
	: _html(other._html)
{
}

HtmlBuilder& HtmlBuilder::operator=(const HtmlBuilder& other)
{
	if (this != &other)
		_html = other._html;
	return *this;
}

HtmlBuilder& HtmlBuilder::operator+(const HtmlBuilder& other)
{
	if (this != &other)
		_html = _html + other._html;
	return *this;
}

HtmlBuilder::~HtmlBuilder()
{
}

HtmlBuilder& HtmlBuilder::Tag(const std::string& tag)
{
	_html = "<" + tag + ">\n" + _html + "</" + tag + ">\n";
	return *this;
}

const std::string& HtmlBuilder::str() const
{
	return _html;
}
