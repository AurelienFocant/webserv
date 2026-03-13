#ifndef UPLOAD_HPP
#define UPLOAD_HPP

#include <string>

class Response;
class Request;
class PathContext;

namespace upload
{
	bool			hasContentTypeHeader(const Request& req);
	bool			isMultiformData(const Request& req);
	std::string		extractBoundary(const Request& req);
	std::string		extractFilename(const Request& req, std::string boundary);
	std::string		verifyFile(const PathContext& ctx, std::string filename);
	bool			saveDataToFile(const Request& req, std::string filename);
}

#endif