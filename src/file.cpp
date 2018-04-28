#include "file.hpp"
#include "current_dir.hpp"

File::File(Current_dir* parent, const std::string& name, char type, unsigned size)
	: parent(parent), name(name), type(type), size(size) 
{
}

File File::rename(const std::string& new_name) &&
{
	// propratni efekat?!
	fs::path parent_path = parent->getPath();
	
	try 
	{ 
		fs::rename(parent_path / name, parent_path / new_name);
	}
	catch(const fs::filesystem_error& e) 
	{
		// TODO -> Sta kod greske
		std::cerr << "Error while renaming the file:" << std::endl;
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	File tmp(std::forward<File>(*this));
	tmp.name = new_name;
	return tmp;
}

File File::rename(const std::string& new_name) const &
{

	fs::path parent_path = parent->getPath();

	// propratni efekat?!
	try 
	{
		fs::rename(parent_path / name, parent_path / new_name);
	} 
	catch (const fs::filesystem_error& e) 
	{
		// TODO -> Sta kod greske
		std::cerr << "Error while renaming the file:" << std::endl;
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}	

	return File(parent, new_name, type, size); 
}

const std::string& File::get_name() const
{
	return name;
}

const std::string File::get_info() const
{
	std::ostringstream info;
	info << name << " " << type << " " << size;
	return info.str();
}

