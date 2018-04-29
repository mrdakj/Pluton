#include "../include/file.hpp"

File::File(const std::string& name, char type, unsigned size)
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

char File::get_type() const
{
	return type;
}

unsigned File::get_size() const
{
	return size;
}

const std::string File::get_info() const
{
	std::ostringstream info;
	info << name << " " << type << " " << size;
	return info.str();
}

