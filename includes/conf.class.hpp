#ifndef CONFIG_CLASS_HPP
# define CONFIG_CLASS_HPP
#include "common.hpp"

class Conf
{
public:
	Conf(std::ofstream fs);
	~Conf();
private:
	Conf();
};

#endif