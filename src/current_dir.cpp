#include "current_dir.hpp"
#include "system.hpp"

#define ERROR_PATH "error"
#define ERROR_DIR Current_dir(ERROR_PATH)

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

void Current_dir::set_error_dir()
{
	dir_path = ERROR_PATH;
}

Current_dir::Current_dir(const std::string& dir_path, immer::flex_vector<File> dirs, immer::flex_vector<File> regular_files)
	: dir_path(dir_path), dirs(std::move(dirs)), regular_files(std::move(regular_files))
{
	if (dir_path == ERROR_PATH || !fs::is_directory(dir_path))
		set_error_dir();
}

Current_dir::Current_dir(const std::string& dir_path)
	: dir_path(dir_path)
{
	if (dir_path == ERROR_PATH || !fs::is_directory(dir_path)) {
		set_error_dir();
	}
	else {
		try {
			for (auto&& p : fs::directory_iterator(dir_path)) {
				std::string file_name = p.path().filename().string();

				if (fs::is_directory(p)) {
					File f(file_name, 'd', std::distance(fs::directory_iterator(p), fs::directory_iterator{}));
					auto index = binary_lower(f.get_name(), dirs);
					dirs = std::move(dirs).insert(index,std::move(f));
				}
				else if (fs::is_regular_file(p)) {
					File f(file_name, 'r', fs::file_size(p));
					auto index = binary_lower(f.get_name(), regular_files);
					regular_files = std::move(regular_files).insert(index,std::move(f));
				}
			}
		}
		catch(...) {
			set_error_dir();
		}
	}
}

bool Current_dir::is_error_dir() const
{
	return (dir_path == ERROR_PATH);
}


Current_dir Current_dir::cd(const fs::path& dir_path) const
{
	if (!fs::is_directory(dir_path))
		return ERROR_DIR;

	return Current_dir(dir_path);
}


Current_dir Current_dir::rename(const File& f, const std::string& new_file_name) const
{
	// if the file name already exists return an error dir
	if (get_file_index(new_file_name) != get_num_of_files())
		return ERROR_DIR;

	if (f.is_dir()) {
		// rename a dir
		auto index = get_dir_index(f.get_name());

		if (index != dirs.size()) {
			auto new_data = dirs.erase(index);
			auto new_place = binary_lower(new_file_name, new_data);
			new_data = std::move(new_data).insert(new_place, dirs[index].rename(new_file_name));

			return Current_dir(dir_path, std::move(new_data), regular_files);
		}
	}
	else if (f.is_regular()) {
		// rename a regular file
		auto index = get_regular_file_index(f.get_name());

		if (index != regular_files.size()) {
			auto new_data = regular_files.erase(index);
			auto new_place = binary_lower(new_file_name, new_data);
			new_data = std::move(new_data).insert(new_place, regular_files[index].rename(new_file_name));

			return Current_dir(dir_path, dirs, std::move(new_data));
		}
	}

	// not a dir and not a regular file
	return ERROR_DIR;
}


Current_dir Current_dir::insert_file(const File& f) const
{
	if (f.is_dir()) {
		// insert a dir
		auto place_to_insert = binary_lower(f.get_name(), dirs);

		if (place_to_insert < get_num_of_dirs() && dirs[place_to_insert].get_name() == f.get_name()) {
			// dir with the same name
			return ERROR_DIR;
		}
		else if (get_regular_file_index(f.get_name()) != get_num_of_regular_files()) {
			// regular file with the same name
			return ERROR_DIR;
		}

		auto new_data = dirs.insert(place_to_insert, f);
		return Current_dir(dir_path, std::move(new_data), regular_files);
	}
	else if (f.is_regular()) {
		// insert a regular file
		auto place_to_insert = binary_lower(f.get_name(), regular_files);

		if (place_to_insert < get_num_of_regular_files() && regular_files[place_to_insert].get_name() == f.get_name()) {
			// regular file with the same name
			return ERROR_DIR;
		}
		else if (get_dir_index(f.get_name()) != get_num_of_dirs()) {
			// dir with the same name
			return ERROR_DIR;
		}

		auto new_data = regular_files.insert(place_to_insert, f);
		return Current_dir(dir_path, dirs, std::move(new_data));
	}

	// not a dir and not a regular file
	return ERROR_DIR;
}

Current_dir Current_dir::delete_file(const File& f) const
{
	if (f.is_dir()) {
		// delete a dir
		auto index_of_file = get_dir_index(f.get_name());

		if (index_of_file == get_num_of_dirs()) {
			// dir doesn't exist
			return ERROR_DIR;
		}

		return Current_dir(dir_path, dirs.erase(index_of_file), regular_files);
	}
	else if (f.is_regular()) {
		// delete a regular file
		auto index_of_file = get_regular_file_index(f.get_name());

		if (index_of_file == get_num_of_regular_files()) {
			// regular file doesn't exist
			return ERROR_DIR;
		}
		
		return Current_dir(dir_path, dirs, regular_files.erase(index_of_file));
	}

	// not a dir and not a regular file
	return ERROR_DIR;
}

std::size_t Current_dir::get_file_index(const std::string &file_name) const
{
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
}

std::size_t Current_dir::get_regular_file_index(const std::string& file_name) const
{
	return binary_search(file_name, regular_files);
}

std::size_t Current_dir::get_dir_index(const std::string& file_name) const
{
	return binary_search(file_name, dirs);
}

std::size_t Current_dir::get_num_of_regular_files() const
{
	return regular_files.size();
}

std::size_t Current_dir::get_num_of_dirs() const
{
	return dirs.size();
}

std::size_t Current_dir::get_num_of_files() const
{
	return get_num_of_dirs() + get_num_of_regular_files();
}

const fs::path& Current_dir::get_path() const
{
	return dir_path;
}

const File& Current_dir::get_file_by_index(unsigned i) const
{
	if (i < get_num_of_dirs())
		return dirs[i];
	
	return regular_files[i-dirs.size()];
}

const File& Current_dir::get_regular_file_by_index(unsigned i) const
{
	return regular_files[i];
}

const File& Current_dir::get_dir_by_index(unsigned i) const
{
	return dirs[i];
}
