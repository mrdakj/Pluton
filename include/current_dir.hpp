#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <immer/flex_vector.hpp>
#include <experimental/filesystem>
#include "optional_ref.hpp"
#include "file.hpp"
#include <variant>
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

		optional_ref<const fs::path> path() const;

		std::size_t num_of_files() const;
		std::size_t num_of_regular_files() const;
		std::size_t num_of_dirs() const;

		// get a file from an imaginary vector dirs+regular_files
		optional_ref<const file> file_by_index(unsigned i) const;
		// get a file from regular_files
		optional_ref<const file> regular_file_by_index(unsigned i) const;
		// get a file from dirs
		optional_ref<const file> dir_by_index(unsigned i) const;

		// get an index from an imaginary vector dirs+regular_files
		std::size_t file_index(const std::string& file_name) const;
		// get an index from regular_files
		std::size_t regular_file_index(const std::string& file_name) const;
		// get an index from dirs
		std::size_t dir_index(const std::string& file_name) const;

		current_dir cd(const fs::path& dir_path) const;
		current_dir rename(const file& f, const std::string& new_file_name) const;
		current_dir insert_file(const file& f) const;
		current_dir delete_file(const file& f) const;

		bool is_error_dir() const;
};

#endif /* CURRENT_DIR_HPP */
