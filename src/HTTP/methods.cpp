
#include "methods.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "RequestHandler.hpp"
#include "HTTPenum.hpp"
#include "Location.hpp"
#include "resp.hpp"
#include "autoindex.hpp"
#include "../Utils/fileSystem.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

/* METHOD */
namespace method
{
	bool	dispatch(PathContext& ctx, Request& req, Response& resp)
	{
		switch(req.getMethod())
		{
			case GET:
				return processGet(ctx, resp);
			case HEAD:
				return processHead(ctx, resp);
			case POST:
				return processPost(ctx, req, resp);
			case DELETE:
				return processDelete(ctx, resp);
			default: 
				resp.setStatusCode(METHOD_NOT_ALLOWED); 
				return false;
		}
		return (true);
	}

	bool	isAllowed(const PathContext& ctx, Request& req, Response& resp)
	{
		std::set<std::string> allowed = ctx.matched_location->getAllowedMethods();

		if (allowed.empty())
			return true;

		std::string method_str = methodToString(req.getMethod());

		if (allowed.find(method_str) == allowed.end())
		{
			req.setStatusCode(METHOD_NOT_ALLOWED);
			resp.setHeader("Allow", resp::buildAllowHeader(allowed));
			return false;
		}
		return true;
	}

	bool	processGet(PathContext& ctx, Response& resp)
	{
		if (ctx.is_directory)
		{
			if (!resolveIndex(ctx))
			{
				if (hasAutoIndex(ctx))
				{
					generateAutoIndex(ctx, resp);
						return true;
				}
				resp.setStatusCode(NOT_FOUND);
				return false;
			}
		}

		if (!resp::loadBody(resp, ctx.resolved_path))
			return false;
		
/* 		if (resp.getMethod() == NOT_SET)
			resp.setMethod(GET); */

		return true;	
	}

	bool	processHead(PathContext& ctx, Response& resp)
	{
		resp.setMethod(HEAD);
		processGet(ctx, resp);
		return (false);
	}

	bool	processPost(PathContext& ctx, const Request& req, Response& resp)
	{
		if (upload::isMultiformData(req))
		{
			if (!upload::hasContentTypeHeader(req)) {
				resp.setStatusCode(BAD_REQUEST);	return (false);
			}

			std::string boundary = upload::extractBoundary(req);
			if (boundary.empty()) {
				resp.setStatusCode(BAD_REQUEST);	return (false);
			}

			std::string filename = upload::extractFilename(req, boundary);
			if (filename.empty()) {
				resp.setStatusCode(BAD_REQUEST);	return (false);
			}

			filename = upload::verifyFile(ctx, filename);
			if (filename.empty()) {
				resp.setStatusCode(FORBIDDEN);		return (false);
			}

			if (!upload::saveDataToFile(req, filename)) {
				resp.setStatusCode(INTERNAL_SERVER_ERROR);	return (false);
			}

			resp.setStatusCode(CREATED);
			return (true);
		}

		resp.setStatusCode(OK);
		return (true);
	}

	bool	processDelete(const PathContext& ctx, Response& resp)
	{
		if (ctx.is_directory) {
			resp.setStatusCode(FORBIDDEN); return (false);
		}

		if (!fileSystem::isFile(ctx.resolved_path)) {
			resp.setStatusCode(NOT_FOUND); return (false);
		}

		std::string dirname	= fileSystem::getDirname(ctx.resolved_path);
		if (!fileSystem::isWritable(dirname) || !fileSystem::isExecutable(dirname)) {
			resp.setStatusCode(FORBIDDEN); return (false);
		}

		if (unlink(ctx.resolved_path.c_str()) != 0) {
			resp.setStatusCode(FORBIDDEN); return (false);
		}

		resp.setStatusCode(NO_CONTENT);
		return (true);
	}

	/* INDEX/DIRECTORY HANDLING */
	bool	resolveIndex(PathContext& ctx)
	{
		const std::vector<std::string> indexes = ctx.matched_location->getIndexes();

		std::string	dir_path = ctx.resolved_path;
		if (!dir_path.empty() && dir_path[dir_path.size() -1] != '/')
			dir_path += '/';

		for (size_t i = 0; i < indexes.size(); i++)
		{
			std::string test_path = dir_path + indexes[i];
			if (access(test_path.c_str(), R_OK) == 0)
			{
				ctx.resolved_path = test_path;
				ctx.is_directory = false;
				return true;
			}
		}
		return false;
	}

	bool	hasAutoIndex(const PathContext& ctx)
	{
		return ctx.matched_location->getAutoIndex();
	}

	void	generateAutoIndex(const PathContext& ctx, Response& resp)
	{
		std::string	html = ::generateAutoIndex(ctx.resolved_path);

		resp.setBody(html);
		resp.setHeader("Content-Type", "text/html");
	}
}

/* UPLOAD */
namespace upload
{
	bool	hasContentTypeHeader(const Request& req)
	{
		if (req.getHeaderValues("Content-Type").size()) {
			if (!req.getHeaderValues("Content-Type")[0].empty())
				return (true);
		}
		return false;
	}

	bool	isMultiformData(const Request& req)
	{
		size_t	pos = std::string::npos;

		std::string	header = req.getHeaderValues("Content-Type")[0];
		if ((pos = header.find("multipart/form-data")) != std::string::npos) {
			return (true);
		}
		return (false);
	}

	std::string	extractBoundary(const Request& req)
	{
		size_t	pos = std::string::npos;
		std::string boundary_str(" boundary=");

		std::string	header = req.getHeaderValues("Content-Type")[0];
		if ((pos = header.find(boundary_str)) != std::string::npos) {
			std::string res = header.substr(pos + boundary_str.size());
			return (res);
		}
		return ("");
	}

	std::string	extractFilename(const Request& req, std::string boundary)
	{
		size_t start_of_filename;
		size_t end_of_filename;
		std::string body = req.getBody();

		if (!body.find(boundary))
			return "";

		std::string file("filename=");
		start_of_filename = body.find(file);
		if (start_of_filename == std::string::npos)
			return "";

		start_of_filename += file.size();
		end_of_filename = body.find("\r\n", start_of_filename);
		if (end_of_filename == std::string::npos)
			return "";

		if (body[start_of_filename] == '"') {
			if (body[end_of_filename - 1] != '"')
				return "";
			start_of_filename++;
			end_of_filename--;
		}

		std::string filename = body.substr(start_of_filename, end_of_filename - start_of_filename);
		return (filename);
	}

	std::string	verifyFile(const PathContext& ctx, std::string filename)
	{
		std::string dir = ctx.resolved_path;
		if (!fileSystem::isDir(dir))
			return ("");

		if (dir[dir.size() - 1] != '/')
			dir = dir + "/";

		if (!fileSystem::isExecutable(dir) || !fileSystem::isWritable(dir))
			return ("");

		filename = dir + filename;
		if (!fileSystem::isFile(filename))
			return (filename);

		if (!fileSystem::isReadable(filename) || !fileSystem::isWritable(filename))
			return ("");

		if (filename.find("../") != std::string::npos)
			return ("");

		return (filename);
	}

	bool	saveDataToFile(const Request& req, std::string filename)
	{
		std::string body = req.getBody();

		size_t end_of_header = body.find("\r\n\r\n");
		if (end_of_header == std::string::npos)
			return (false);
		end_of_header += 4;

		int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd < 0)
			return (false);

		std::string s_buffer = body.substr(end_of_header);

		int bytes_sent = 0;
		int start = 0;
		while ((bytes_sent = write(fd, &(s_buffer.c_str()[start]), 4000)) > 0) {
			start += bytes_sent;
		}
		close(fd);
		return (true);
	}

}
