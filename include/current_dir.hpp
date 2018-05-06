#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <immer/flex_vector.hpp>
#include <immer/algorithm.hpp>
#include <algorithm>
#include <experimental/filesystem>
#include "file.hpp"
#include <fstream>
#include <iostream>

#define unused(x) ((void)x)

namespace fs = std::experimental::filesystem;

class Current_dir {
	// TODO -> Potrebni su dirs i regular_files
	// fukciji koja dobavlja sadrzaj direktorijuma
	// u klasi File_manager_tui
	friend class File_manager_tui;
private:
	fs::path path;
	immer::flex_vector<File> dirs;
	immer::flex_vector<File> regular_files;

public:

	Current_dir(const std::string& path, immer::flex_vector<File> dirs, immer::flex_vector<File> regular_files);
	Current_dir(const std::string& path);

	immer::flex_vector<File> ls() const;

	Current_dir cd(const File& dir) const;
	Current_dir cd(fs::path dir_path) const;

	Current_dir rename(const File& f, const std::string& new_file_name) const &;
	Current_dir rename(const File& f, const std::string& new_file_name) &&;

	Current_dir insert_file(File&& f) const &;
	Current_dir insert_file(File&& f) &&;


	Current_dir delete_file(const File& f) const &;
	Current_dir delete_file(const File& f) &&;

	const fs::path& get_path() const;

	const File find_by_fname(const std::string &file_name) const;
	int get_index_by_name(const std::string &file_name) const;
};

#endif /* CURRENT_DIR_HPP */
