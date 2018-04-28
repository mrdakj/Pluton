#ifndef FILE_HPP
#define FILE_HPP 

#include <string>
#include <sstream>
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;


class File {
private:
	std::string name;
	char type;
	unsigned size;
public:
	File(const std::string& name, char type = 'r', unsigned size = 0);

	File rename(const std::string& new_name) &&;
	File rename(const std::string& new_name) const &;

	const std::string& get_name() const;
	const std::string get_info() const;
};

#endif /* FILE_HPP */
