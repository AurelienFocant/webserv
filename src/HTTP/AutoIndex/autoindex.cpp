#include <stdexcept>
#include <vector>
#include <string>

#include <dirent.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>

#include "HtmlBuilder.hpp"
#include "../Utils/fileSystem.hpp"

bool compareFilenames(const std::string& a, const std::string& b)
{
	bool aIsDir = fileSystem::isDir(a);
	bool bIsDir = fileSystem::isDir(b);
	if (aIsDir && !bIsDir) return true;
	if (!aIsDir && bIsDir) return false;
	return a < b;
}

std::vector<std::string>	resolveLocalDirFilenames(std::string const& path)
{
	DIR						*dirp;
	struct dirent			*dirent;
	std::vector<std::string> filenames;

	dirp = opendir(path.c_str());
	if (!dirp)
		throw (std::runtime_error("Couldn't open dir"));	// should probably return 500 internal error
	// !!!!!!!!!!!!!!!!!!!!!!!! //


	filenames.push_back("../");
	while (((dirent) = readdir(dirp))) {
		std::string file(dirent->d_name);
		if (file.at(0) != '.') {
			if (fileSystem::isDir(path + "/" + file))
				file += '/';
			filenames.push_back(file);
		}
	}

	closedir(dirp);


	std::sort(filenames.begin(), filenames.end(), compareFilenames);
	return (filenames);
}

std::string	buildHtmlBody(std::vector<std::string> & filenames)
{
	std::string body;

	if (!filenames.empty()) {
		std::vector<std::string>::iterator it;

		for (it = filenames.begin(); it != filenames.end(); ++it) {
			*it =
				"<a href=\"" + *it + "\">"
				+ *it + "</a>";
		}
		for (it = filenames.begin(); it != filenames.end(); ++it) {
			body += *it + '\n';
		}
	}
	return (body);
}

std::string	addAutoindexHtml(std::string const& path, std::string const& body)
{
	HtmlBuilder html(body);
	HtmlBuilder	title("Index of " + path + '\n');
	
	HtmlBuilder css("<link rel=\"stylesheet\" href=\"/css/autoindex.css\">\n");
	HtmlBuilder header = title;
	header + css;
	header.Tag("head");

	title.Tag("title").Tag("head");
	header.Tag("h1");

	html.Tag("pre")
		.Tag("hr");

	html = header + html;
	html.Tag("body");

	html = title + html;
	html.Tag("html");
	return (html.str());
}

std::string generateAutoIndex(std::string const& path)
{
	std::vector<std::string>	filenames	= resolveLocalDirFilenames(path.c_str());
	std::string					body		= buildHtmlBody(filenames);
	return (addAutoindexHtml(path, body));
}
