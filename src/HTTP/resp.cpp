#include "resp.hpp"
#include "Response.hpp"
#include "RequestHandler.hpp"
#include "VirtualServer.hpp"
#include "SessionManager.hpp"
#include "../Utils/httpUtils.hpp"
#include "../Utils/fileSystem.hpp"

#include <sstream>
#include <fstream>
#include <fcntl.h>

namespace resp
{
	std::string	hardcode500Error(void)
	{
		return ("\
				<!DOCTYPE html>\n\
				<html lang=\"en\">\n\
				<head>\n\
				<meta charset=\"UTF-8\">\n\
				<title>500 - Internal Server Error</title>\n\
				</head>\n\
				<body>\n\
				<div class=\"container\">\n\
				<h1>WebServ 500</h1>\n\
				<p>Who changed the permissions of my file ?</p>\n\
				<a href=\"/\">Back to Home</a>\n\
				</div>\n\
				</body>\n\
				</html>\n\
				");
	}

	void	prepareResponse(RequestHandler & reqHandler)
	{
		Response& response = reqHandler.getResponse();
		if (response.getStatusCode() >= 400 && response.getStatusCode() != METHOD_NOT_ALLOWED)
		{
			int status_code = response.getStatusCode();
			const std::map<int, std::string> error_pages = reqHandler.getVirtualServer()->getErrorPages();
			std::string body;

			if (!resp::loadErrorPage(reqHandler, status_code, error_pages, body)) {
				body = hardcode500Error();
			}
			response.setBody(body);
			response.setHeader("Content-Type", "text/html");
		}

		const Request& request = reqHandler.getRequest();
		response.setHttpVersion(request.getHttpVersion());
		response.setHeader("Date", httpUtils::getTime());
		response.setHeader("Server", "webservMeBaby");

		std::vector<std::string> header_values = request.getHeaderValues("connection");
		std::string connection;
		if (header_values.empty() || header_values[0].empty())
			connection = (response.getHttpVersion() == "HTTP/1.1") ? "keep-alive" : "close";
		else
		{
			std::string value = header_values[0];
			for (size_t i = 0; i < value.size(); i++)
				value[i] = std::tolower(value[i]);

			if (value == "keep-alive" || value == "close")
				connection = value;
			else
				connection = (response.getHttpVersion() == "HTTP/1.1") ? "keep-alive" : "close";
		}

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

	bool	loadErrorPage(RequestHandler &reqHandler, int status_code, const std::map<int, std::string>& error_pages, std::string& body)
	{
		if (status_code == METHOD_NOT_ALLOWED)
			return true ;

		std::map<int, std::string>::const_iterator it = error_pages.find(status_code);
		if (it == error_pages.end())
			return false;
		std::string error_page = it->second;

		if (error_page.find("./") != 0)
		{
			Location const* location = reqHandler.getCtx().matched_location;
			if (!location->getAlias().empty())
				error_page = location->getAlias() + error_page;
			else
				error_page = location->getRoot() + error_page;
		}

		int fd = open(error_page.c_str(), O_RDONLY);
		if (fd == -1) {
			status_code = INTERNAL_SERVER_ERROR;
			it = error_pages.find(INTERNAL_SERVER_ERROR);
			fd = open(it->second.c_str(), O_RDONLY);
			if (fd == -1)
				return (false);
		}
		close(fd);
		loadFileToString(error_page, body);
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

	std::string	extractCookie(const Request& request, const std::string& cookie_name)
	{
		std::vector<std::string> cookie = request.getHeaderValues("Cookie");
		if (cookie.empty() || cookie[0].empty())
			return "";
		
		size_t pos = cookie[0].find(cookie_name + "=");
		if (pos == std::string::npos)
			return "";

		pos += cookie_name.size() + 1;
		size_t end = cookie[0].find(";", pos);

		std::string result = (end == std::string::npos)
			? cookie[0].substr(pos) 
			: cookie[0].substr(pos, end - pos);
		
		return result;
	}

void	handleSession(const Request& request, Response& response)
	{
		SessionManager& manager = SessionManager::createManager();

		std::string id = extractCookie(request, "sessionId");
		std::string	client_id = manager.handleId(id);

		if (client_id == id)
		{
			if (std::difftime(std::time(NULL), manager.getCookie(id).created_at) > session_lifetime)
			{
/* 					char buf[64];
				std::time_t expired = manager.getCookie(id).expired;
				std::tm *tm = std::gmtime(&expired);
				strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);

				std::string cookie = "sessionId= ; Path=/; expires=" + std::string(buf);
				response.setHeader("Set-Cookie", cookie); */
				manager.deleteCookie(id);
				//response.setHeader("Set-Cookie", "sessionId=" + client_id + "; Path=/ ; SameSite=Lax");
				client_id = manager.handleId("");
			}
		}
		response.setHeader("Set-Cookie", "sessionId=" + client_id + "; Path=/; SameSite=Lax");
		manager.incrementValue(client_id, response);
	}
}
