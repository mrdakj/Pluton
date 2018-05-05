#include "../include/current_dir.hpp"
#include "../include/system.hpp"

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
Current_dir::Current_dir(const std::string& path, immer::flex_vector<File> dirs, immer::flex_vector<File> regular_files)
	: path(fs::absolute(path)), dirs(std::move(dirs)), regular_files(std::move(regular_files))
{
	/* TODO FIX check error */
	try  {
		if (!fs::is_directory(path)) {
			std::cerr << "Error: " << path << " not a directory" << std::endl;
			exit(EXIT_FAILURE);
		}
	} catch (const fs::filesystem_error &e) {
		std::cerr << "Error while instanciating current_dir: " << std::endl;
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

Current_dir::Current_dir(const std::string& path)
	: path(fs::absolute(path))
{
	/* TODO FIX check error */
	try  {
		if (!fs::is_directory(path)) {
			std::cerr << "Error: " << path << " not a directory" << std::endl;
			exit(EXIT_FAILURE);
		}
	} catch (const fs::filesystem_error &e) {
		std::cerr << "Error while instanciating current_dir: " << std::endl;
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	//FIX make more functional
	for (auto & p : fs::directory_iterator(path)) {

		std::string file_name = p.path().filename().string();

		if (fs::is_directory(p)) {
			File f(file_name, 'd', std::distance(fs::directory_iterator(p), fs::directory_iterator{}));
			unsigned index = binary_lower(f.get_name(), dirs);
			dirs = std::move(dirs).insert(index,std::move(f));
		}
		else {
			File f(file_name, 'r', fs::file_size(p));
			unsigned index = binary_lower(f.get_name(), regular_files);
			regular_files = std::move(regular_files).insert(index,std::move(f));
		}
	}
}

immer::flex_vector<File> Current_dir::ls() const
{
	return dirs + regular_files;
}

Current_dir Current_dir::cd(const File& dir) const
{
	fs::path dir_path = path / dir.get_name();

	bool is_dir;
	try {
		is_dir = fs::is_directory(dir_path);
	} catch (const fs::filesystem_error& e) {
		return *this;
	}

	if (!is_dir) {
		throw "nod a dir";
		return *this;
	}

	Current_dir new_dir = *this;
	try {
		new_dir =  Current_dir(dir_path);
	} catch (...) {
		return *this;
	}

	return new_dir;
}

Current_dir Current_dir::cd(fs::path dir_path) const
{
	fs::path dpath = fs::absolute(dir_path);

	bool is_dir;
	try {
		is_dir = fs::is_directory(dir_path);
	} catch (const fs::filesystem_error& e) {
		// TODO: Sending error to TUI -> Permission denied or something ..
		return *this;
	}

	if (!is_dir) {
		throw "nod a dir";
		return *this;
	}


	Current_dir new_dir = *this;
	try {
		new_dir =  Current_dir(dpath);
	} catch (const fs::filesystem_error &e) {
		// TODO: Sending error to TUI -> Permission denied or something ..
		return *this;
	}

	return new_dir;
}




Current_dir Current_dir::rename(const File& f, const std::string& new_file_name) const &
{
	sys::rename_on_system(path / f.get_name(), path / new_file_name);

	if (f.get_type() == 'd') {
		unsigned index = binary_search(f.get_name(), dirs);
		if (index < dirs.size()) {
			auto new_data = dirs.erase(index);
			int new_place = binary_lower(new_file_name, dirs);
			new_data = std::move(new_data).insert(new_place, dirs[index].rename(new_file_name));
			return Current_dir(path, std::move(new_data), regular_files);
		}
	}
	else {
		unsigned index = binary_search(f.get_name(), regular_files);
		if (index < regular_files.size()) {
			auto new_data = regular_files.erase(index);
			int new_place = binary_lower(new_file_name, regular_files);
			new_data = std::move(new_data).insert(new_place, regular_files[index].rename(new_file_name));
			return Current_dir(path, dirs, std::move(new_data));
		}
	}

	return *this;
}

Current_dir Current_dir::rename(const File& f, const std::string& new_file_name) &&
{
	unused(f);
	unused(new_file_name);
	//TODO implement this
	return *this;
}

const fs::path& Current_dir::get_path() const
{
	return path;
}



Current_dir Current_dir::insert_file(File&& f) const &
{
	if (f.get_type() == 'd') {
		unsigned place = binary_lower(f.get_name(), dirs);
		if (place < dirs.size() && dirs[place].get_name() == f.get_name()) {
			std::cerr << "dir exists" << std::endl;
			return *this;
		}
		else {
			sys::insert_dir_on_system(path / f.get_name());
		}
		auto new_data = dirs.insert(place, std::forward<File>(f));
		return Current_dir(path, std::move(new_data), regular_files);
	}
	else {
		unsigned place = binary_lower(f.get_name(), regular_files);
		if (place < regular_files.size() && regular_files[place].get_name() == f.get_name()) {
			std::cerr << "file exists" << std::endl;
			return *this;
		}
		else {
			sys::insert_rfile_on_system(path / f.get_name());
		}
		auto new_data = regular_files.insert(place, std::forward<File>(f));
		return Current_dir(path, dirs, std::move(new_data));
	}
}

Current_dir Current_dir::insert_file(File&& f) &&
{
	unused(f);
	//TODO implement this
	return *this;
}


Current_dir Current_dir::delete_file(const File& f) const &
{
	sys::remove_from_system(path / f.get_name());

	if (f.get_type() == 'd') {
		unsigned index = binary_search(f.get_name(), dirs);
		if (index >= dirs.size()) {
			std::cerr << "dir doesnt exist" << std::endl;
			return *this;
		}
		return Current_dir(path, dirs.erase(index), regular_files);
	}
	else {
		unsigned index = binary_search(f.get_name(), regular_files);
		if (index >= regular_files.size()) {
			std::cerr << "regular file doesnt exist" << std::endl;
			return *this;
		}
		return Current_dir(path, dirs, regular_files.erase(index));
	}
}

Current_dir Current_dir::delete_file(const File& f) &&
{
	unused(f);
	//TODO implement this
	return *this;
}


const File Current_dir::find_by_fname(const std::string &file_name) const
{
	auto num_of_files = regular_files.size();
	if (num_of_files > 0) {	
		auto index = binary_search(file_name, regular_files);
		if (regular_files[index].get_name() == file_name)
			return regular_files[index];
	}

	auto num_of_dirs = dirs.size();
	if (num_of_dirs > 0) {	
		auto index = binary_search(file_name, regular_files);
		index = binary_search(file_name, dirs);

		if (dirs[index].get_name() == file_name)
			return dirs[index];

		// if nothing found and num_of_dirs > 0 return first dir
		return dirs[0];
	}
	
	// if nothing found and num_of_files > 0 return first file
	if (num_of_files > 0)
		return regular_files[0];
	
	// If nothing found return empty file object
	return File("");
}
