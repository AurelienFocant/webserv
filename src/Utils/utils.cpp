#include "RequestHandler.hpp"

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

size_t fileSize(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return false;
	return statBuf.st_size;
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

bool isExecutable(const std::string& path)
{
/* 	int	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return false;
	close(fd);
	return true; */
	return (access(path.c_str(), X_OK) == 0);

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
