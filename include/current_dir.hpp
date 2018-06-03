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


class current_dir {
private:
	struct data {
		fs::path dir_path;
		immer::flex_vector<file> dirs;
		immer::flex_vector<file> regular_files;
	};


	std::variant<data, std::string> m_data;

	current_dir(const std::string& dir_path, immer::flex_vector<file> dirs, immer::flex_vector<file> regular_files);
public:
	current_dir(const std::string& dir_path, bool error_flag = false);

	std::optional<std::reference_wrapper<const fs::path>> get_path() const;

	std::size_t get_num_of_files() const;
	std::size_t get_num_of_regular_files() const;
	std::size_t get_num_of_dirs() const;

	// get a file from an imaginary vector dirs+regular_files
	std::optional<std::reference_wrapper<const file>> get_file_by_index(unsigned i) const;
	// get a file from regular_files
	std::optional<std::reference_wrapper<const file>> get_regular_file_by_index(unsigned i) const;
	// get a file from dirs
	std::optional<std::reference_wrapper<const file>> get_dir_by_index(unsigned i) const;

	// get an index from an imaginary vector dirs+regular_files
	std::size_t get_file_index(const std::string& file_name) const;
	// get an index from regular_files
	std::size_t get_regular_file_index(const std::string& file_name) const;
	// get an index from dirs
	std::size_t get_dir_index(const std::string& file_name) const;

	current_dir cd(const fs::path& dir_path) const;
	current_dir rename(const file& f, const std::string& new_file_name) const;
	current_dir insert_file(const file& f) const;
	current_dir delete_file(const file& f) const;

	bool is_error_dir() const;
};

#endif /* CURRENT_DIR_HPP */
