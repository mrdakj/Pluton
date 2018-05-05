#ifndef SYSTEM_HPP
#define SYSTEM_HPP 

#include <experimental/filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace sys {
	void rename_on_system(const fs::path& name, const fs::path& new_name);
	void insert_rfile_on_system(const fs::path& name);
	void insert_dir_on_system(const fs::path& name);
	std::uintmax_t remove_from_system(const fs::path& name);
}

#endif /* SYSTEM_HPP */
