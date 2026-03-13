#ifndef UPLOAD_HPP
#define UPLOAD_HPP

#include "upload.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "PathContext.hpp"
#include "../Utils/fileSystem.hpp"
#include "HTTPenum.hpp"

#include <fcntl.h>
#include <unistd.h>

namespace upload
{
	bool	hasContentTypeHeader(const Request& req)
	{
		if (req.getHeaderValues("Content-Type").size()) {
			if (!req.getHeaderValues("Content-Type")[0].empty())
				return (true);
		}
		return (false);
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

#endif