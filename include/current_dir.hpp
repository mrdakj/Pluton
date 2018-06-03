#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <immer/flex_vector.hpp>
#include <experimental/filesystem>
#include "file.hpp"
#include <variant>
#include <functional>
#include <iostream>
#include <optional>

namespace fs = std::experimental::filesystem;


class Current_dir {
private:
	struct data {
		fs::path dir_path;
		immer::flex_vector<File> dirs;
		immer::flex_vector<File> regular_files;
	};


	std::variant<data, std::string> m_data;

	Current_dir(const std::string& dir_path, immer::flex_vector<File> dirs, immer::flex_vector<File> regular_files);
public:
	Current_dir(const std::string& dir_path, bool error_flag = false);

	std::optional<std::reference_wrapper<const fs::path>> get_path() const;

	std::size_t get_num_of_files() const;
	std::size_t get_num_of_regular_files() const;
	std::size_t get_num_of_dirs() const;

	// get a file from an imaginary vector dirs+regular_files
	std::optional<std::reference_wrapper<const File>> get_file_by_index(unsigned i) const;
	// get a file from regular_files
	std::optional<std::reference_wrapper<const File>> get_regular_file_by_index(unsigned i) const;
	// get a file from dirs
	std::optional<std::reference_wrapper<const File>> get_dir_by_index(unsigned i) const;

	// get an index from an imaginary vector dirs+regular_files
	std::size_t get_file_index(const std::string& file_name) const;
	// get an index from regular_files
	std::size_t get_regular_file_index(const std::string& file_name) const;
	// get an index from dirs
	std::size_t get_dir_index(const std::string& file_name) const;

	Current_dir cd(const fs::path& dir_path) const;
	Current_dir rename(const File& f, const std::string& new_file_name) const;
	Current_dir insert_file(const File& f) const;
	Current_dir delete_file(const File& f) const;

	bool is_error_dir() const;
};

#endif /* CURRENT_DIR_HPP */
