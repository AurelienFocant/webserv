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
