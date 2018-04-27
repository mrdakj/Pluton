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
	File(const std::string& name, char type = 'r', unsigned size = 0)
		: name(name), type(type), size(size) 
	{
	}

	File rename(const std::string& new_name) &&
	{
		// propratni efekat?!
		fs::rename(name, new_name);
		
		File tmp(std::forward<File>(*this));
		tmp.name = new_name;
		return tmp;
	}

	File rename(const std::string& new_name) const &
	{
		// propratni efekat?!
		fs::rename(name, new_name);
		return File(new_name, type, size); 
	}

	const std::string& get_name() const
	{
		return name;
	}

	const std::string get_info() const
	{
		std::ostringstream info;
		info << name << " " << type << " " << size;
		return info.str();
	}

};
#endif /* FILE_HPP */
