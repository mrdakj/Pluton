#include "current_dir.hpp"
#include "system.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#define unused(x) ((void)x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template<class ForwardIt, class T, class Compare=std::less<>>
static ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp={})
{
    first = std::lower_bound(first, last, value, comp);
    return first != last && !comp(value, *first) ? first : last;
}

static std::size_t binary_search(const std::string& name, immer::flex_vector<File> v)
{
	auto it = binary_find(v.begin(), v.end(), File(name), [](auto&& arg1, auto&& arg2) { return arg1.get_name() < arg2.get_name(); });
    return std::distance(v.begin(), it);
}

static std::size_t binary_lower(const std::string& name, immer::flex_vector<File> v)
{
	auto it = std::lower_bound(v.begin(), v.end(), File(name), [](auto&& arg1, auto&& arg2) { return arg1.get_name() < arg2.get_name(); });
    return std::distance(v.begin(), it);
}

Current_dir::Current_dir(const std::string& dir_path, immer::flex_vector<File> dirs, immer::flex_vector<File> regular_files)
{
	if (!fs::is_directory(dir_path)) {
		m_data = std::string("not a dir");
	}
	else {
		Current_dir::data data;
		data.dirs = std::move(dirs);
		data.regular_files = std::move(regular_files);
		data.dir_path = dir_path;

		m_data = std::move(data);
	}
}

Current_dir::Current_dir(const std::string& dir_path, bool error_flag)
{
	if (error_flag) {
		// dir path here is an error message
		m_data = std::string(dir_path);
		return;
	}

	if (!fs::is_directory(dir_path)) {
		m_data = std::string("not a dir");
	}
	else {
		try {
			Current_dir::data data;
			data.dir_path = dir_path;
			for (auto&& p : fs::directory_iterator(dir_path)) {
				std::string file_name = p.path().filename().string();

				if (fs::is_directory(p)) {
					File f(file_name, DIRECTORY, std::distance(fs::directory_iterator(p), fs::directory_iterator{}));
					auto index = binary_lower(f.get_name(), data.dirs);
					data.dirs = std::move(data.dirs).insert(index,std::move(f));
				}
				else if (fs::is_regular_file(p)) {
					File f(file_name, REGULAR, fs::file_size(p));
					auto index = binary_lower(f.get_name(), data.regular_files);
					data.regular_files = std::move(data.regular_files).insert(index,std::move(f));
				}
			}

			m_data = std::move(data);
		}
		catch(...) {
			m_data = std::string("an error occured");
		}
	}
}

bool Current_dir::is_error_dir() const
{
	try {
		std::get<std::string>(m_data);
		return true;
	}
	catch (...) {
		return false;
	}
}


Current_dir Current_dir::cd(const fs::path& dir_path) const
{
	if (!fs::is_directory(dir_path))
		return Current_dir("not a dir", true);

	return Current_dir(dir_path);
}


Current_dir Current_dir::rename(const File& f, const std::string& new_file_name) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
					// if the file name already exists return an error dir
					if (get_file_index(new_file_name) != get_num_of_files())
						return Current_dir("file name already exists", true);

					if (f.is_dir()) {
						// rename a dir
						auto index = get_dir_index(f.get_name());

						if (index != data.dirs.size()) {
							auto new_data = data.dirs.erase(index);
							auto new_place = binary_lower(new_file_name, new_data);
							new_data = std::move(new_data).insert(new_place, data.dirs[index].rename(new_file_name));
							return Current_dir(data.dir_path, std::move(new_data), data.regular_files);
						}
					}
					else if (f.is_regular()) {
						// rename a regular file
						auto index = get_regular_file_index(f.get_name());

						if (index != data.regular_files.size()) {
							auto new_data = data.regular_files.erase(index);
							auto new_place = binary_lower(new_file_name, new_data);
							new_data = std::move(new_data).insert(new_place, data.regular_files[index].rename(new_file_name));

							return Current_dir(data.dir_path, data.dirs, std::move(new_data));
						}
					}

					// not a dir and not a regular file
					return Current_dir("not a dir and not a regular file", true);
			},
			[](const std::string& e) {
				unused(e);
				return Current_dir("operation on error dir", true);
			}
    }, m_data);
}


Current_dir Current_dir::insert_file(const File& f) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				if (f.is_dir()) {
					// insert a dir
					auto place_to_insert = binary_lower(f.get_name(), data.dirs);

					if (place_to_insert < get_num_of_dirs() && data.dirs[place_to_insert].get_name() == f.get_name()) {
						// dir with the same name
						return Current_dir("dir with the same name", true);
					}
					else if (get_regular_file_index(f.get_name()) != get_num_of_regular_files()) {
						// regular file with the same name
						return Current_dir("regular file with the same name", true);
					}

					auto new_data = data.dirs.insert(place_to_insert, f);
					return Current_dir(data.dir_path, std::move(new_data), data.regular_files);
				}
				else if (f.is_regular()) {
					// insert a regular file
					auto place_to_insert = binary_lower(f.get_name(), data.regular_files);

					if (place_to_insert < get_num_of_regular_files() && data.regular_files[place_to_insert].get_name() == f.get_name()) {
						// regular file with the same name
						return Current_dir("regular file with the same name", true);
					}
					else if (get_dir_index(f.get_name()) != get_num_of_dirs()) {
						// dir with the same name
						return Current_dir("dir with the same name", true);
					}

					auto new_data = data.regular_files.insert(place_to_insert, f);
					return Current_dir(data.dir_path, data.dirs, std::move(new_data));
				}

				// not a dir and not a regular file
				return Current_dir("not a dir and not a regular file", true);
			},
			[](const std::string& e) {
				unused(e);
				return Current_dir("operation on error dir", true);
			}
    }, m_data);
}

Current_dir Current_dir::delete_file(const File& f) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				if (f.is_dir()) {
					// delete a dir
					auto index_of_file = get_dir_index(f.get_name());

					if (index_of_file == get_num_of_dirs()) {
						// dir doesn't exist
						return Current_dir("dir doesn't exist", true);
					}

					return Current_dir(data.dir_path, data.dirs.erase(index_of_file), data.regular_files);
				}
				else if (f.is_regular()) {
					// delete a regular file
					auto index_of_file = get_regular_file_index(f.get_name());

					if (index_of_file == get_num_of_regular_files()) {
						// regular file doesn't exist
						return Current_dir("regular file doesn't exist", true);
					}
					
					return Current_dir(data.dir_path, data.dirs, data.regular_files.erase(index_of_file));
				}

				// not a dir and not a regular file
				return Current_dir("not a dir and not a regular file", true);
			},
			[](const std::string& e) {
				unused(e);
				return Current_dir("operation on error dir", true);
			}
    }, m_data);
}

std::size_t Current_dir::get_file_index(const std::string &file_name) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				unused(data);

				// try to find a file in regular files
				auto index_of_file = get_regular_file_index(file_name);

				if (index_of_file != get_num_of_regular_files())
					return get_num_of_dirs() + index_of_file;

				// try to find a file in dirs
				index_of_file = get_dir_index(file_name);

				if (index_of_file != get_num_of_dirs())
					return index_of_file;

				// file not found
				return get_num_of_files();
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t Current_dir::get_regular_file_index(const std::string& file_name) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				return binary_search(file_name, data.regular_files);
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t Current_dir::get_dir_index(const std::string& file_name) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				return binary_search(file_name, data.dirs);
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t Current_dir::get_num_of_regular_files() const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				return data.regular_files.size();
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t Current_dir::get_num_of_dirs() const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				return data.dirs.size();
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t Current_dir::get_num_of_files() const
{
	return get_num_of_dirs() + get_num_of_regular_files();
}

std::optional<std::reference_wrapper<const fs::path>> Current_dir::get_path() const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				return std::optional<std::reference_wrapper<const fs::path>>{data.dir_path};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const fs::path>>)std::nullopt;
			}
    }, m_data);
}

std::optional<std::reference_wrapper<const File>> Current_dir::get_file_by_index(unsigned i) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				if (i < get_num_of_dirs())
					return std::optional<std::reference_wrapper<const File>>{data.dirs[i]};
				
				return std::optional<std::reference_wrapper<const File>>{data.regular_files[i-data.dirs.size()]};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const File>>)std::nullopt;
			}
    }, m_data);
}

std::optional<std::reference_wrapper<const File>> Current_dir::get_regular_file_by_index(unsigned i) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				return std::optional<std::reference_wrapper<const File>>{data.regular_files[i]};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const File>>)std::nullopt;
			}
    }, m_data);
}

std::optional<std::reference_wrapper<const File>> Current_dir::get_dir_by_index(unsigned i) const
{
	return std::visit(overloaded {
			[&](const Current_dir::data& data) {
				return std::optional<std::reference_wrapper<const File>>{data.dirs[i]};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const File>>)std::nullopt;
			}
    }, m_data);
}
