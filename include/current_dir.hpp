#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <immer/vector.hpp>
#include <immer/flex_vector.hpp>
#include <immer/algorithm.hpp>
#include <range/v3/all.hpp>
#include <algorithm>
#include <experimental/filesystem>
#include <iostream>
#include "file.hpp"


namespace fs = std::experimental::filesystem;

class Current_dir {
private:

	fs::path path;
	immer::flex_vector<File> data;

	void rename_on_system(const std::string& file_name, const std::string& new_file_name) const;
	void insert_on_system(const File& f) const;
	std::uintmax_t remove_from_system(const std::string& file_name) const;
	unsigned file_search(const std::string& file_name) const;

public:
	Current_dir(const std::string& path, immer::flex_vector<File> data);
	Current_dir(const std::string& path);

	immer::flex_vector<File> ls() const;

	Current_dir cd(const std::string& dir) const;

	Current_dir rename(const std::string& file_name, const std::string& new_file_name) const &;
	Current_dir rename(const std::string& file_name, const std::string& new_file_name) &&;

	Current_dir insert_file(File&& f) const &;
	Current_dir insert_file(File&& f) &&;


	Current_dir delete_file(const std::string& file_name) const &;
	Current_dir delete_file(const std::string& file_name) &&;

	const fs::path& get_path() const;
};

#endif /* CURRENT_DIR_HPP */
