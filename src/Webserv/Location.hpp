#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <ctime>

class ConfigContext;

class Location 
{
	private:
	std::string _name;
	std::string _root;
	std::string _alias;
	std::string	_redirect;
	int			_redirect_code;
	bool		_autoindex;
	std::vector<std::string>	_indexes;
	bool		_cgi_on;
	std::time_t	_keepalive_time;
	std::time_t	_keepalive_timeout;

	public:

	/* Getters */
	std::string					getName() const {return _name;}
	std::string					getRoot() const {return _root;}
	std::string					getAlias() const {return _alias;}
	bool						getCGI() const {return _cgi_on;}
	std::string					getRedirect() const {return _redirect;}
	int							getRedirectCode() const {return _redirect_code;}
	std::vector<std::string>	getIndexes() const {return _indexes;}
	bool						getAutoIndex() const {return _autoindex;}


	/* Setters */
	void	setName(const std::string& name) {_name = name;}
	void	setRoot(const std::string& root) {_root = root;}
	void	setAlias(const std::string& alias) {_alias = alias;}
	void	setIndexes(const std::vector<std::string>& indexes) {_indexes = indexes;}
	void	setAutoIndex(bool autoindex) {_autoindex = autoindex;}


	Location	();
	Location	(ConfigContext& ctxt);
	Location	(const Location& other);
	Location&	operator= (const Location& rhs);
	~Location	();
};

#endif // LOCATION_HPP
