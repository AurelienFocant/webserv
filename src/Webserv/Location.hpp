#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <ctime>
#include <set>

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
	std::time_t	_keepalive_time;
	std::time_t	_keepalive_timeout;

	bool		_cgi_on;
	//bool		_virtual;

	std::vector<std::string>	_indexes;
	std::set<std::string>		_allowed_methods;


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
	std::set<std::string>	getAllowedMethods(void) const	{return _allowed_methods;}
	void					setAllowedMethods(std::set<std::string> m) {_allowed_methods = m;}


	Location	(ConfigContext& ctxt);
	Location	(const Location& other);
	Location&	operator= (const Location& rhs);
	~Location	();
};

#endif // LOCATION_HPP
