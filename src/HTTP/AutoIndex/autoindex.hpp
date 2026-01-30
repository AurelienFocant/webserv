#pragma once

#include <string>
#include <vector>

std::string 				generateAutoIndex(std::string const& path);
std::vector<std::string>	resolveLocalDirFilenames(std::string const& path);
