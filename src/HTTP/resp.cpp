#include "resp.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "../Utils/httpUtils.hpp"
#include "../Utils/fileSystem.hpp"

#include <sstream>
#include <fstream>

namespace resp
{
	void	prepareResponse(Response& response, const Request& request, const std::map<int, std::string>& error_pages)
	{
		int status_code = response.getStatusCode();

		if (response.getStatusCode() >= 400 /* && response.getBody().empty() */)
		{
			std::string body;
			if (!resp::loadErrorPage(status_code, error_pages, body))
			{
				// do stuff;
				// 500 error?
			}
			response.setBody(body);
			response.setHeader("Content-Type", "text/html");
		}

		response.setHttpVersion(request.getHttpVersion());
		response.setHeader("Date", httpUtils::getTime());
		//response.setHeader("Server", "webservMeBaby;")
		//content-length? set in Response when the body_size is known in every case

		std::vector<std::string> header_values = request.getHeaderValues("connection");
		std::string connection;
		if (header_values.empty() || header_values[0].empty())
			connection = (response.getHttpVersion() == "HTTP/1.1") ? "keep-alive" : "close";
		else 
			connection = header_values[0];

		response.setHeader("Connection", connection);
		response.setState(Response::READY);
	}

	bool	loadBody(Response& response, const std::string& path)
	{
		std::string body;
		if (!loadFileToString(path, body))
		{
			response.setStatusCode(httpUtils::errnoToHttpStatus(errno));
			return false;
		}
		response.setBody(body);
		response.setHeader("Content-Type", fileSystem::getContentType(path));

		return true;
	}

	bool 	loadFileToString(const std::string& path, std::string& body)
	{
		std::ifstream file(path.c_str(), std::ios::binary);
		if (!file.is_open())
			return false;
		std::stringstream ss;
		ss << file.rdbuf(); //lire tout le fichier en une fois
		if (file.fail())
			return false;

		body = ss.str();
		return true;
}

	bool	loadErrorPage(int status_code, const std::map<int, std::string>& error_pages, std::string& body)
	{
		std::map<int, std::string>::const_iterator it = error_pages.find(status_code);
		if (it == error_pages.end())
			return false;

		// ADD check if absolute path or string to concatene
/* 		std::string error_path;
		retrieve _root from RH
		it->second[0] == '/' ? error_path = it->second : error_path = root + it->second; */

		loadFileToString(it->second, body);
		return true;
	}

	std::string		buildAllowHeader(const std::set<std::string>& allowed_methods)
	{
		std::string allow_header;
		for (std::set<std::string>::const_iterator it = allowed_methods.begin();
			it != allowed_methods.end(); it++)
		{
			if (it != allowed_methods.begin())
				allow_header += ", ";
			allow_header += *it;
		}
		return allow_header;
	}
}
