#include "fileSystem.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace fileSystem {

	bool isFile(const std::string& path)
	{
		struct stat statBuf;
		if (stat(path.c_str(), &statBuf) != 0)
			return false;
		return S_ISREG(statBuf.st_mode);
	}

	bool isDir(const std::string& path)
	{
		struct stat statBuf;
		if (stat(path.c_str(), &statBuf) != 0)
			return false;
		return S_ISDIR(statBuf.st_mode);
	}

	bool isReadable(const std::string& path)
	{
	/* 	int	fd = open(path.c_str(), O_RDONLY);
		if (fd < 0)
			return false;
		close(fd);
		return true; */
		return (access(path.c_str(), R_OK) == 0);
	}

	bool isWritable(const std::string& path)
	{
	/* 	int	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
			return false;
		close(fd);
		return true; */
		return (access(path.c_str(), W_OK) == 0);

	}

	size_t fileSize(const std::string& path)
	{
		struct stat statBuf;
		if (stat(path.c_str(), &statBuf) != 0)
			return 0;
		return statBuf.st_size;
	}

	std::string	getContentType(const std::string& path)
	{
		size_t dotPos = path.find_last_of('.');
		if (dotPos == std::string::npos) {
			return "application/octet-stream";
		}
		
		std::string ext = path.substr(dotPos + 1);
		
		if (ext == "html" || ext == "htm")return "text/html";
		else if (ext == "css") return "text/css";
		else if (ext == "js") return "application/javascript";
		else if (ext == "json") return "application/json";
		else if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
		else if (ext == "png") return "image/png";
		else if (ext == "gif") return "image/gif";
		else if (ext == "txt") return "text/plain";
		else if (ext == "pdf") return "application/pdf";
		
		return "application/octet-stream";
	}

	int	openReadFile(const std::string& path)
	{
		int	fd = open(path.c_str(), O_RDONLY);
		return fd;
	}

	int	openWriteFile(const std::string& path)
	{
		int	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644); // CHECK FLAGS -> APPEND?
		return fd;
	}

}