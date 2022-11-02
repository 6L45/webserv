#ifndef CONFIG_CLASS_HPP
# define CONFIG_CLASS_HPP
#include "common.hpp"

/* 
	Macro to access the bit of options 
		SC_DIRISACTIVE return true if directory browser is allowed, false otherwise
		SC_BODYISLIMITED return true if body required a max size, false otherwise
		SC_HTMLGETISSET, SC_HTMLPORTISSET , SC_HTMLDELETEISSET return true if the method is authorisez, false otherwise
*/
#define SC_DIRISACTIVE(_sc) (_sc.options & (1 << 0))
#define SC_BODYISLIMITED(_sc) (_sc.options & (1 << 1))
#define SC_GETISSET(_sc) (_sc.methods & (1 << 0))
#define SC_PORTISSET(_sc) (_sc.methods & (1 << 1))
#define SC_DELETEISSET(_sc) (_sc.methods & (1 << 2))

/*
	Macro to set the bit of options 
*/
#define _SC_DIRACTIVATE(_sc) (_sc.options = _sc.options | (1 << 0))
#define _SC_BODYLIMITACTIVATE(_sc) (_sc.options = _sc.options | (1 << 1))
#define _SC_GETACTIVATE(_sc) (_sc.methods = _sc.methods | (1 << 0))
#define _SC_PORTACTIVATE(_sc) (_sc.methods = _sc.methods | (1 << 1))
#define _SC_DELETEACTIVATE(_sc) (_sc.methods = _sc.methods | (1 << 2))

class __server_conf
{
public:
	std::string					server_name;
	int							port;
	//std::vector<int>			port;
	std::string					root;
	std::vector<std::string>	host;
	char						options;
	char						methods;
	int							body_limits;
	int							unactive_max_delay;
};

class Conf
{
public:
	Conf(std::ifstream &fs);
	Conf(std::string path_to_file);
	~Conf();


private:
	std::vector<__server_conf>	_sc;
	int							_max_connexion;
	std::string					_default_error_root;
	int							_line_read;
	
	Conf();

	void						__parse_config_file(std::ifstream& fs);
	void						__parse_server(std::ifstream &fs, std::string& line);
	template<class T>
	void						__add_to(T& to, std::string& s);
	void 						__get_info(int &c, std::string& raw);
	void						__get_info(std::string& c, std::string& raw);
	void						__error_file_notif(const int line, const std::string& error) const;
	std::string::iterator		__erase_tab_space(std::string& s) const;
	std::string&				__erase_comment(std::string& s) const;
	std::string&				__erase_from_char(std::string& s, char c) const;

	void						__print_everything() const;
	void						__get_line(std::ifstream &fs, std::string& line);
	bool						__valide_conf_file_name(const std::string& name) const;
};

#endif