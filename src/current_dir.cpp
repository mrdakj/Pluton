#include "current_dir.hpp"
#include "system.hpp"

#define ERROR_PATH "error"
#define ERROR_DIR Current_dir(ERROR_PATH)

template<class ForwardIt, class T, class Compare=std::less<>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp={})
{
    first = std::lower_bound(first, last, value, comp);
    return first != last && !comp(value, *first) ? first : last;
}

unsigned binary_search(const std::string& name, immer::flex_vector<File> v)
{
	auto it = binary_find(v.begin(), v.end(), File(name), [](auto&& arg1, auto&& arg2) { return arg1.get_name() < arg2.get_name(); });

    return std::distance(v.begin(), it);
}

unsigned binary_lower(const std::string& name, immer::flex_vector<File> v)
{
	auto it = std::lower_bound(v.begin(), v.end(), File(name), [](auto&& arg1, auto&& arg2) { return arg1.get_name() < arg2.get_name(); });

    return std::distance(v.begin(), it);
}

Current_dir::Current_dir(const std::string& dir_path, immer::flex_vector<File> dirs, immer::flex_vector<File> regular_files)
{
	if (dir_path == ERROR_PATH || !fs::is_directory(dir_path)) {
		this->dir_path = ERROR_PATH;
		return;
	}

	this->dir_path = fs::absolute(dir_path); 
	this->dirs = std::move(dirs);
	this->regular_files = std::move(regular_files);
}

Current_dir::Current_dir(const std::string& dir_path)
{

	if (dir_path == ERROR_PATH || !fs::is_directory(dir_path)) {
		this->dir_path = ERROR_PATH;
		return;
	}

	this->dir_path = fs::absolute(dir_path); 

	try
	{
		//FIX make more functional
		for (auto & p : fs::directory_iterator(this->dir_path)) {

			std::string file_name = p.path().filename().string();

			if (fs::is_directory(p)) {
				File f(file_name, 'd', std::distance(fs::directory_iterator(p), fs::directory_iterator{}));
				unsigned index = binary_lower(f.get_name(), dirs);
				dirs = std::move(dirs).insert(index,std::move(f));
			}
			else if (fs::is_regular_file(p)) {
				File f(file_name, 'r', fs::file_size(p));
				unsigned index = binary_lower(f.get_name(), regular_files);
				regular_files = std::move(regular_files).insert(index,std::move(f));
			}
		}
	}
	catch(...) {
		this->dir_path = ERROR_PATH;
		return;
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



const File& Current_dir::get_file_by_index(unsigned i) const
{
	if (i < dirs.size())
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

Current_dir Current_dir::rename(const File& f, const std::string& new_file_name) const
{
	if (binary_search(new_file_name, dirs) < dirs.size() || binary_search(new_file_name, regular_files) < regular_files.size())
		return ERROR_DIR;

	if (f.get_type() == 'd') {
		unsigned index = binary_search(f.get_name(), dirs);
		if (index < dirs.size()) {
			auto new_data = dirs.erase(index);
			int new_place = binary_lower(new_file_name, new_data);
			new_data = std::move(new_data).insert(new_place, dirs[index].rename(new_file_name));
			return Current_dir(dir_path, std::move(new_data), regular_files);
		}
	}
	else {
		unsigned index = binary_search(f.get_name(), regular_files);
		if (index < regular_files.size()) {
			auto new_data = regular_files.erase(index);
			int new_place = binary_lower(new_file_name, new_data);
			new_data = std::move(new_data).insert(new_place, regular_files[index].rename(new_file_name));
			return Current_dir(dir_path, dirs, std::move(new_data));
		}
	}

	return *this;
}

const fs::path& Current_dir::get_path() const
{
	return dir_path;
}



Current_dir Current_dir::insert_file(const File& f) const
{
	if (f.get_type() == 'd') {
		unsigned place = binary_lower(f.get_name(), dirs);
		if (place < dirs.size() && dirs[place].get_name() == f.get_name()) {
			// dir with the same name
			return ERROR_DIR;
		}
		else if (binary_search(f.get_name(), regular_files) < regular_files.size()) {
			// file with the same name
			return ERROR_DIR;
		}
		auto new_data = dirs.insert(place, f);
		return Current_dir(dir_path, std::move(new_data), regular_files);
	}
	else {
		unsigned place = binary_lower(f.get_name(), regular_files);
		if (place < regular_files.size() && regular_files[place].get_name() == f.get_name()) {
			return ERROR_DIR;
		}
		else if (binary_search(f.get_name(), dirs) < dirs.size()) {
			// dir with the same name
			return ERROR_DIR;
		}
		auto new_data = regular_files.insert(place, f);
		return Current_dir(dir_path, dirs, std::move(new_data));
	}

	return ERROR_DIR;
}


Current_dir Current_dir::delete_file(const File& f) const
{
	if (f.get_type() == 'd') {
		unsigned index = binary_search(f.get_name(), dirs);
		if (index >= dirs.size()) {
			// dir doesnt exist
			return ERROR_DIR;
		}
		return Current_dir(dir_path, dirs.erase(index), regular_files);
	}
	else {
		unsigned index = binary_search(f.get_name(), regular_files);
		if (index >= regular_files.size()) {
			// regular file doesnt exist
			return ERROR_DIR;
		}
		return Current_dir(dir_path, dirs, regular_files.erase(index));
	}

	return ERROR_DIR;
}


std::size_t Current_dir::get_index_by_name(const std::string &file_name) const
{
	auto index = binary_search(file_name, regular_files);
	if (index != regular_files.size())
		return dirs.size() + index;

	index = binary_search(file_name, dirs);
	if (index != dirs.size())
		return index;

	return get_num_of_files();
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
	return regular_files.size() + dirs.size();
}
