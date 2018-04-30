#include "../include/current_dir.hpp"

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
	: path(path), dirs(std::move(dirs)), regular_files(std::move(regular_files))
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
	: path(path)
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
	if (!fs::is_directory(dir_path))
		throw "nod a dir";
	return Current_dir(dir_path);
}

void Current_dir::rename_on_system(const File& f, const std::string& new_file_name) const
{
	try { 
		// side effect?!
		fs::rename(path / f.get_name(), path / new_file_name);
	}
	catch(const fs::filesystem_error& e) {
		// TODO -> what to do? 
		std::cerr << "Error while renaming the file:" << std::endl;
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}



Current_dir Current_dir::rename(const File& f, const std::string& new_file_name) const &
{
	rename_on_system(f, new_file_name);

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


void Current_dir::insert_on_system(const File& f) const
{
	// TODO check errors and check if a file already exists
	if (f.get_type() == 'd')
		fs::create_directory(path / f.get_name());
	if (f.get_type() == 'r')
		std::ofstream(path / f.get_name());
}

Current_dir Current_dir::insert_file(File&& f) const &
{
	insert_on_system(f);
	if (f.get_type() == 'd') {
		unsigned place = binary_lower(f.get_name(), dirs);
		if (place < dirs.size() && dirs[place].get_name() == f.get_name()) {
			std::cerr << "dir exists" << std::endl;
			return *this;
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

std::uintmax_t Current_dir::remove_from_system(const File& f) const
{
	return fs::remove_all(path / f.get_name());
}

Current_dir Current_dir::delete_file(const File& f) const &
{
	remove_from_system(f);

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


