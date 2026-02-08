#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

namespace fileSystem {

	bool	 	isFile(const std::string& path);

	bool		isDir(const std::string& path);

	bool		isReadable(const std::string& path);

	bool 		isWritable(const std::string& path);

	int			openReadFile(const std::string& path);

	int			openWriteFile(const std::string& path);

	size_t	 	fileSize(const std::string& path);

	std::string	getContentType(const std::string& path);

}

#endif