#include "current_dir.hpp"
#include "system.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <range/v3/view.hpp>
#include <range/v3/action/sort.hpp>
#define unused(x) ((void)x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template<class ForwardIt, class T, class Compare=std::less<>>
static ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp={})
{
    first = std::lower_bound(first, last, value, comp);
    return first != last && !comp(value, *first) ? first : last;
}

static std::size_t binary_search(const std::string& name, immer::flex_vector<file> v)
{
	auto it = binary_find(v.begin(), v.end(), file(name), [](auto&& arg1, auto&& arg2) { return arg1.get_name() < arg2.get_name(); });
    return std::distance(v.begin(), it);
}

static std::size_t binary_lower(const std::string& name, immer::flex_vector<file> v)
{
	auto it = std::lower_bound(v.begin(), v.end(), file(name), [](auto&& arg1, auto&& arg2) { return arg1.get_name() < arg2.get_name(); });
    return std::distance(v.begin(), it);
}

current_dir::current_dir(const std::string& dir_path, immer::flex_vector<file> dirs, immer::flex_vector<file> regular_files)
{
	if (!fs::is_directory(dir_path)) {
		m_data = std::string("not a dir");
	}
	else {
		current_dir::data data;
		data.dirs = std::move(dirs);
		data.regular_files = std::move(regular_files);
		data.dir_path = dir_path;

		m_data = std::move(data);
	}
}

current_dir::current_dir(const std::string& dir_path, bool error_flag)
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
			current_dir::data data;
			data.dir_path = dir_path;

			fs::directory_iterator ls { dir_path };
			auto regular_files = ls 
						| ranges::v3::view::filter([] (const fs::directory_entry& entry) { return fs::is_regular_file(entry.path()); })
						| ranges::v3::view::transform([](const fs::path& path) { return file(path.filename(), REGULAR, fs::file_size(path)); })
						| ranges::v3::to_vector
						| ranges::v3::action::sort([](const file& a, const file& b) { return a.get_name() < b.get_name(); });

			data.regular_files = immer::flex_vector<file>{regular_files.begin(), regular_files.end()};

			fs::directory_iterator ls2 { dir_path };
			auto dirs = ls2 
						| ranges::v3::view::filter([] (const fs::directory_entry& entry) { return fs::is_directory(entry.path()); })
						| ranges::v3::view::transform([](const fs::path& path) { return file(path.filename(), DIRECTORY, std::distance(fs::directory_iterator(path), fs::directory_iterator{})); })
						| ranges::v3::to_vector
						| ranges::v3::action::sort([](const file& a, const file& b) { return a.get_name() < b.get_name(); });

			data.dirs = immer::flex_vector<file>{dirs.begin(), dirs.end()};

			m_data = std::move(data);
		}
		catch(...) {
			m_data = std::string("an error occured");
		}
	}
}

bool current_dir::is_error_dir() const
{
	try {
		std::get<std::string>(m_data);
		return true;
	}
	catch (...) {
		return false;
	}
}


current_dir current_dir::cd(const fs::path& dir_path) const
{
	if (!fs::is_directory(dir_path))
		return current_dir("not a dir", true);

	return current_dir(dir_path);
}


current_dir current_dir::rename(const file& f, const std::string& new_file_name) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
					// if the file name already exists return an error dir
					if (get_file_index(new_file_name) != get_num_of_files())
						return current_dir("file name already exists", true);

					if (f.is_dir()) {
						// rename a dir
						auto index = get_dir_index(f.get_name());

						if (index != data.dirs.size()) {
							auto new_data = data.dirs.erase(index);
							auto new_place = binary_lower(new_file_name, new_data);
							new_data = std::move(new_data).insert(new_place, data.dirs[index].rename(new_file_name));
							return current_dir(data.dir_path, std::move(new_data), data.regular_files);
						}
					}
					else if (f.is_regular()) {
						// rename a regular file
						auto index = get_regular_file_index(f.get_name());

						if (index != data.regular_files.size()) {
							auto new_data = data.regular_files.erase(index);
							auto new_place = binary_lower(new_file_name, new_data);
							new_data = std::move(new_data).insert(new_place, data.regular_files[index].rename(new_file_name));

							return current_dir(data.dir_path, data.dirs, std::move(new_data));
						}
					}

					// not a dir and not a regular file
					return current_dir("not a dir and not a regular file", true);
			},
			[](const std::string& e) {
				unused(e);
				return current_dir("operation on error dir", true);
			}
    }, m_data);
}


current_dir current_dir::insert_file(const file& f) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				if (f.is_dir()) {
					// insert a dir
					auto place_to_insert = binary_lower(f.get_name(), data.dirs);

					if (place_to_insert < get_num_of_dirs() && data.dirs[place_to_insert].get_name() == f.get_name()) {
						// dir with the same name
						return current_dir("dir with the same name", true);
					}
					else if (get_regular_file_index(f.get_name()) != get_num_of_regular_files()) {
						// regular file with the same name
						return current_dir("regular file with the same name", true);
					}

					auto new_data = data.dirs.insert(place_to_insert, f);
					return current_dir(data.dir_path, std::move(new_data), data.regular_files);
				}
				else if (f.is_regular()) {
					// insert a regular file
					auto place_to_insert = binary_lower(f.get_name(), data.regular_files);

					if (place_to_insert < get_num_of_regular_files() && data.regular_files[place_to_insert].get_name() == f.get_name()) {
						// regular file with the same name
						return current_dir("regular file with the same name", true);
					}
					else if (get_dir_index(f.get_name()) != get_num_of_dirs()) {
						// dir with the same name
						return current_dir("dir with the same name", true);
					}

					auto new_data = data.regular_files.insert(place_to_insert, f);
					return current_dir(data.dir_path, data.dirs, std::move(new_data));
				}

				// not a dir and not a regular file
				return current_dir("not a dir and not a regular file", true);
			},
			[](const std::string& e) {
				unused(e);
				return current_dir("operation on error dir", true);
			}
    }, m_data);
}

current_dir current_dir::delete_file(const file& f) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				if (f.is_dir()) {
					// delete a dir
					auto index_of_file = get_dir_index(f.get_name());

					if (index_of_file == get_num_of_dirs()) {
						// dir doesn't exist
						return current_dir("dir doesn't exist", true);
					}

					return current_dir(data.dir_path, data.dirs.erase(index_of_file), data.regular_files);
				}
				else if (f.is_regular()) {
					// delete a regular file
					auto index_of_file = get_regular_file_index(f.get_name());

					if (index_of_file == get_num_of_regular_files()) {
						// regular file doesn't exist
						return current_dir("regular file doesn't exist", true);
					}
					
					return current_dir(data.dir_path, data.dirs, data.regular_files.erase(index_of_file));
				}

				// not a dir and not a regular file
				return current_dir("not a dir and not a regular file", true);
			},
			[](const std::string& e) {
				unused(e);
				return current_dir("operation on error dir", true);
			}
    }, m_data);
}

std::size_t current_dir::get_file_index(const std::string &file_name) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
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

std::size_t current_dir::get_regular_file_index(const std::string& file_name) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				return binary_search(file_name, data.regular_files);
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t current_dir::get_dir_index(const std::string& file_name) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				return binary_search(file_name, data.dirs);
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t current_dir::get_num_of_regular_files() const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				return data.regular_files.size();
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t current_dir::get_num_of_dirs() const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				return data.dirs.size();
			},
			[](const std::string& e) {
				unused(e);
				return std::size_t(0);
			}
    }, m_data);
}

std::size_t current_dir::get_num_of_files() const
{
	return get_num_of_dirs() + get_num_of_regular_files();
}

std::optional<std::reference_wrapper<const fs::path>> current_dir::get_path() const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				return std::optional<std::reference_wrapper<const fs::path>>{data.dir_path};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const fs::path>>)std::nullopt;
			}
    }, m_data);
}

std::optional<std::reference_wrapper<const file>> current_dir::get_file_by_index(unsigned i) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				if (i < get_num_of_dirs())
					return std::optional<std::reference_wrapper<const file>>{data.dirs[i]};
				
				return std::optional<std::reference_wrapper<const file>>{data.regular_files[i-data.dirs.size()]};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const file>>)std::nullopt;
			}
    }, m_data);
}

std::optional<std::reference_wrapper<const file>> current_dir::get_regular_file_by_index(unsigned i) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				return std::optional<std::reference_wrapper<const file>>{data.regular_files[i]};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const file>>)std::nullopt;
			}
    }, m_data);
}

std::optional<std::reference_wrapper<const file>> current_dir::get_dir_by_index(unsigned i) const
{
	return std::visit(overloaded {
			[&](const current_dir::data& data) {
				return std::optional<std::reference_wrapper<const file>>{data.dirs[i]};
			},
			[](const std::string& e) {
				unused(e);
				return (std::optional<std::reference_wrapper<const file>>)std::nullopt;
			}
    }, m_data);
}
