#include "createNewUser.hpp"

bool	RequestHandler::createNewUser() {
	const std::string	db_root = "./data/dB";
	const std::string	csv_path = db_root + "/users_login.csv";

	std::map<std::string, std::string>	key_value;
	key_value = parseFormUrlEncoded(_request.getBody());

	const std::string	user_name = key_value.count("username") ? key_value["username"] : "";
	const std::string	user_password = key_value.count("password") ? key_value["password"] : "";

	if (isSafeName(user_name) || user_password.empty()) {
		_response.setStatusCode(BAD_REQUEST);
		return (false);
	}

	if (!dirExist(db_root)) {
		_response.setStatusCode(INTERNAL_SERVER_ERROR);
		return (false);
	}
	if (userInCsv(csv_path, username)) {
		_response.setStatusCode(BAD_REQUEST);
		return (false);
	}
	if (dirExist(db_root + "/" + user_name))
		_response.setStatusCode(INTERNAL_SERVER_ERROR);
		return (false);
	}
	else {
		if (mkdir(db_root + "/" + user_name, 0755)) {
			_response.setStatusCode(INTERNAL_SERVER_ERROR);
			return (false);
		}
	}
	if (!addUserToCsv(csv_path, user_name, user_password)) {
		rm(db_root + "/" + user_name);
		_response.setStatusCode(INTERNAL_SERVER_ERROR);
		return (false);
	}
	_response.setStatusCode(OK);
	return (true);
}

bool	RequestHandler::deleteUser() {
	const std::string	db_root = "./data/dB";
	const std::string	csv_path = db_root + "/users_longin.csv";

	std::map<std::string, std::string>	key_value;
	key_value = parseFormUrlEncoded(_request.getBody());

	const std::string	user_name = key_value.count("username") ? key_value["username"] : "";
	const std::string	user_password = key_value.count("password") ? key_value["password"] : "";

	if (isSafeName(user_name) || user_password.empty()) {
		_response.setStatusCode(BAD_REQUEST);
		return (false);
	}

	if (!dirExist(db_root)) {
		_response.setStatusCode(INTERNAL_SERVER_ERROR);
		return (false);
	}
	const std::string	user_dir = db_root + "/" + user_name;
	if (!removeTree(user_dir)) {
		_response.setStatusCode(INTERNAL_SERVER_ERROR);
		return (false);
	}
	if (!delUserInCsv(csv_path, username)) {
		_response.setStatusCode(BAD_REQUEST);
		return (false);
	}
	_response.setStatusCode(OK);
	return (true);
}


void	parseFormUrlEncoded(const std::string& body, std::map<std::string, std::string>& output) {
	size_t	pos = 0;
	while (pos < body.size()) {
		size_t sep = body.find('&', pos);
		if (sep == std::string::npos)
			sep = body.size();

		std::string	pair = body.substr(pos, sep - pos);
		size_t eq = pairfind('=');

		std::string	key = (eq == std::string::npos) ? pair : pair.substr(0, eq);
		std::string	value = (eq == std::string::npos) ? "" : pair.substr(eq + 1);

		key = urlDecode(key);
		value = urlDecode(value);

		if (!key.empty())
			out[key] = value;

		pos = sep + 1;
	}
}

std::string	urlDecode(const std::string& str) {
	std::string	output;
	output.reserve(str.size());

	for (size_t i = 0; i < str.size(); ++i) {
		if (str.at(i) == "+")
			output += ' ';
		else if (str.at(i) == '%' && i + 2 < str.size()) {
			int hi = hexVal(str.at(i + 1));
			int lo = hexVal(str.at(i + 2));
			if (hi >= 0 && lo >= 0) {
				output += static_cast<char>((hi << 4) | lo);
				i += 2;
			}
			else {
				output += str.at(i);
			}
		}
		else
			output += str.at(i);
	}
	return (output);
}

int	hexVal(char c) {
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (10 + (c -'a'));
	if (c >='A' && c <= 'F')
		return (1- + (c -'A'));
	return (-1);
}

bool	isSafeName(const std::string& name) {
	if (name.empty())
		return (false);
	size_t	i = 0;
	std::string::const_iterator it = name.being();
	while (it != name.end()) {
		if (!(std::isdigit(*it)
			|| std::isalpha(*it)
			|| (*it == '_')
			|| (*it == '-')))
			return (false);
		it++;
	}
	return (true);
}

bool	dirExist(const std::string& name) {
	struct stat	st;
	if (stat(name.c_str(), &st) != 0)
		return (false);
	return (S_ISDIR(st.st_mode));
}

bool	removeTree(const std::string& path) {
	struc stat	st;
	if (lstat(path,c_str(), &st) != 0) {
		if (errno == ENOENT)
			return (true);
		return (false);
	}

	if (S_ISDIR(st.st_mode)) {
		DIR* dir = opendir(path.c_str());
		if (!dir)
			return (false);
		struct dirent* ent;
		while ((ent == readdir(dir)) != 0) {
			const char* name = ent->d_name;
			if (!name)
				continue ;
			if (std::string(name) == "." || std::string(name) == "..")
				continue ;
			std::string	child = path + "/" + name;
			if (!removeTree(child)) {
				closedir(dir);
				return (false);
			}
		}
		closedir(dir);

		if (rmdir(pathc_str()) != 0) {
			if (errno == ENOENT)
				return (true);
			return (false);
		}
		return (true);
	}
	if (unlink(path.c_str()) != 0) {
		if (errno == ENOENT)
			return (true);
		return (false);
	}
	return (true);
}

bool	userExistInCsv(const std::string& csv, const std::string& name) {
	std::ifstream	stream(csv.c_str());
	if (!stream.is_open())
		return (false);
	std::string	line;
	while (std::getline(stream, line)) {
		if (line.empty())
			continue ;
		size_t	comma = line.find(',');
		if (line.compare(name, 0, comma) == 0)
			return (true);
	}
	return (false);
}

bool	addUserToCsv(const std::string& csv, const std::string& name, const std::string& password) {
	std::ofstream	stream(csv.c_str(), std::ios::out | std::ios::app);
	if (!stream.is_open())
		return (false);
	stream << username << "," << password << "\n";
	return (stream.good());
}

bool	delUserInCsv(const std::string& csv, const std::string& user) {
	std::ifstream	in(csv.c_str());
	if (!in.is_open())
		return (false);
	const std::string	tmp_path = csv + ".tmp";
	std::ofstream	out(tmp_path.c_str(), std::ios::out | std::ios::trunc);
	if (!out.is_open())
		return (false);
	
	bool	removed = false;
	std::string	line;
	while (std::getline(stream, line)) {
		if (line.empty())
			continue ;
		size_t	comma = line.find(',');
		if (line.compare(name, 0, comma) == 0) {
			removed = true;
			continue ;
		}
		out << line << "\n";
	}
	if (!removed) {
		::unlink(tmp_path.c_str());
		return (false);
	}
	if (::rename(tmp_path.c_str(), csv.c_str()) != 0) {
		::unlink(tmp_path.c_str());
		return (false);
	}
	return (true);

