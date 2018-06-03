#include "file.hpp"
#include <sstream>


file::file(const std::string& name, file_type type, std::size_t size)
	: name(name), type(type), size(size) 
{
}

file file::rename(const std::string& new_name) &&
{
	file tmp(std::forward<file>(*this));
	tmp.name = new_name;
	return tmp;
}

file file::rename(const std::string& new_name) const &
{
	return file(new_name, type, size); 
}

const std::string& file::get_name() const
{
	return name;
}

file_type file::get_type() const
{
	return type;
}

std::size_t file::get_size() const
{
	return size;
}

const std::string file::get_info() const
{
	if (type != DIRECTORY && type != REGULAR)
		return "Unknown";

	std::ostringstream info;
	info << name << " " << type << " " << size;
	return info.str();
}


bool file::is_dir() const
{
	return type == DIRECTORY;
}

bool file::is_regular() const
{
	return type == REGULAR;
}
