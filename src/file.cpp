#include "file.hpp"
#include <sstream>

File::File(const std::string& name, File_type type, std::size_t size)
	: name(name), type(type), size(size) 
{
}

File File::rename(const std::string& new_name) &&
{
	File tmp(std::forward<File>(*this));
	tmp.name = new_name;
	return tmp;
}

File File::rename(const std::string& new_name) const &
{
	return File(new_name, type, size); 
}

const std::string& File::get_name() const
{
	return name;
}

File_type File::get_type() const
{
	return type;
}

std::size_t File::get_size() const
{
	return size;
}

const std::string File::get_info() const
{
	if (type != DIRECTORY && type != REGULAR)
		return "Unknown";

	std::ostringstream info;
	info << name << " " << type << " " << size;
	return info.str();
}


bool File::is_dir() const
{
	return type == DIRECTORY;
}

bool File::is_regular() const
{
	return type == REGULAR;
}
