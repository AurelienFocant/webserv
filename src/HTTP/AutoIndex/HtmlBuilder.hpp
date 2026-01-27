#ifndef HTMLBUILDER_HPP
#define HTMLBUILDER_HPP

#include <string>

class HtmlBuilder
{
	private:
		std::string _html;

	public:
		HtmlBuilder();
		HtmlBuilder(const std::string& text);
		HtmlBuilder(const HtmlBuilder& other);
		HtmlBuilder& operator=(const HtmlBuilder& other);
		HtmlBuilder& operator+(const HtmlBuilder& other);
		~HtmlBuilder();

		HtmlBuilder& Tag(const std::string& tag);

		const std::string& str() const;
};

#endif
