#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>

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
	std::string	getName() const {return _name;}
	std::string	getRoot() const {return _root;}
	std::string	getAlias() const {return _alias;}

	/* Setters */
	void	setName(const std::string& name) {_name = name;}
	void	setRoot(const std::string& root) {_root = root;}
	void	setAlias(const std::string& alias) {_alias = alias;}

	Location	();
	Location	(const Location& other);
	Location&	operator= (const Location& rhs);
	~Location	();
};

#endif // LOCATION_HPP
