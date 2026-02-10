#include "RequestHandler.hpp"

namespace httpUtils
{
	t_HttpCode errnoToHttpStatus(int error_code)
	{
		switch (error_code) {
		case EACCES:
			return FORBIDDEN;
			break;
		case ENOENT:
			return NOT_FOUND;
		default:
			return INTERNAL_SERVER_ERROR;
		}
	}

	std::string	getTime()
	{
		time_t timestamp;
		std::time(&timestamp);

		struct tm* gmt = std::gmtime(&timestamp);
		char buffer [100];
		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

		return std::string(buffer);
	}

	std::string intToString(size_t value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

}

/* // Not TOCTOU -> open mieux adapte aux permissions
bool isReadable(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return false;
	return (statBuf.st_mode & S_IRUSR) == S_IRUSR;
}

bool isWritable(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return false;
	return (statBuf.st_mode & S_IWUSR) == S_IWUSR;
}

bool isExecutable(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return false;
	return (statBuf.st_mode & S_IXUSR) == S_IXUSR;
} */

/* void RequestHandler::resolvePath()
{
	if (_alias.empty())

	for (std::map<std::string, Location>::iterator it = _config.route.begin(); it != _config.route.end(); i++)
	{
		if (path.find(it->first) == 0)
	}

} */
