#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <immer/flex_vector.hpp>
#include <experimental/filesystem>
#include "file.hpp"

namespace fs = std::experimental::filesystem;

class Current_dir {
private:
	fs::path dir_path;
	immer::flex_vector<File> dirs;
	immer::flex_vector<File> regular_files;

public:
	Current_dir(const std::string& dir_path, immer::flex_vector<File> dirs, immer::flex_vector<File> regular_files);
	Current_dir(const std::string& dir_path);

	const fs::path& get_path() const;

	std::size_t get_num_of_files() const;
	std::size_t get_num_of_regular_files() const;
	std::size_t get_num_of_dirs() const;

	const File& get_file_by_index(unsigned i) const;
	const File& get_regular_file_by_index(unsigned i) const;
	const File& get_dir_by_index(unsigned i) const;

	std::size_t get_index_by_name(const std::string& file_name) const;

	Current_dir cd(const fs::path& dir_path) const;
	Current_dir rename(const File& f, const std::string& new_file_name) const;
	Current_dir insert_file(const File& f) const;
	Current_dir delete_file(const File& f) const;

	bool is_error_dir() const;
};

#endif /* CURRENT_DIR_HPP */
