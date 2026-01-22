#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>

class ConfigContext;

class Location 
{
	private:

	/* Private Attributes */
	std::string _name;
	std::string _root;
	std::string _alias;
	bool		_autoindex;
	std::vector<std::string>	_indexes;

	public:

	/* Getters */
	std::string					getName() const {return _name;}
	std::string					getRoot() const {return _root;}
	std::string					getAlias() const {return _alias;}
	// std::string					getRedirect() const {return _redirect;}
	// int							getRedirectCode() const {return _redirect_code;}
	std::vector<std::string>	getIndexes() const {return _indexes;}
	bool						getAutoIndex() const {return _autoindex;}


	/* Setters */
	void	setName(const std::string& name) {_name = name;}
	void	setRoot(const std::string& root) {_root = root;}
	void	setAlias(const std::string& alias) {_alias = alias;}
	// void	setRedirect(const std::string& redirect) {_redirect = redirect;}
	// void	setRedirectCode(const int& redirect_code) {_redirect_code = redirect_code;}
	void	setIndexes(const std::vector<std::string>& indexes) {_indexes = indexes;}
	void	setAutoIndex(bool autoindex) {_autoindex = autoindex;}

	void	addIndexFile(const std::string& file) {_indexes.push_back(file);}

	Location	();
	Location	(ConfigContext& ctxt);
	Location	(const Location& other);
	Location&	operator= (const Location& rhs);
	~Location	();
};

#endif // LOCATION_HPP
