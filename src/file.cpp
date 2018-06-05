#include "file.hpp"
#include <sstream>


file::file(const std::string& name, file_type type, std::size_t size)
	: m_name(name), m_type(type), m_size(size) 
{
}

file file::rename(const std::string& new_name) &&
{
	file tmp(std::forward<file>(*this));
	tmp.m_name = new_name;
	return tmp;
}

file file::rename(const std::string& new_name) const &
{
	return file(new_name, m_type, m_size); 
}

const std::string& file::name() const
{
	return m_name;
}

file_type file::type() const
{
	return m_type;
}

std::size_t file::size() const
{
	return m_size;
}

const std::string file::info() const
{
	if (m_type != DIRECTORY && m_type != REGULAR)
		return "Unknown";

	std::ostringstream info;
	info << m_name << " " << m_type << " " << m_size;
	return info.str();
}


bool file::is_dir() const
{
	return m_type == DIRECTORY;
}

bool file::is_regular() const
{
	return m_type == REGULAR;
}
