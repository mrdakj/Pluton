#include "../include/system.hpp"
#include <fstream>
#include <iostream>

namespace sys {
	void insert_rfile_on_system(const fs::path& name)
	{
		std::ofstream{name};
	}

	void insert_dir_on_system(const fs::path& name)
	{
		fs::create_directory(name);
	}

	void rename_on_system(const fs::path& name, const fs::path& new_name)
	{
		try { 
			fs::rename(name, new_name);
		}
		catch(const fs::filesystem_error& e) {
			std::cerr << "Error while renaming the file:" << std::endl;
			std::cerr << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	std::uintmax_t remove_from_system(const fs::path& name)
	{
		return fs::remove_all(name);
	}
}
