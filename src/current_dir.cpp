#include "../include/current_dir.hpp"
#include "../include/file.hpp"
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <range/v3/all.hpp>
#include <iostream>

namespace fs = std::experimental::filesystem;

Current_dir::Current_dir(const std::string& path, immer::flex_vector<File> data)
	: path(path), data(std::move(data))
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
		char type = (fs::is_directory(p)) ? 'd' : 'r';
		unsigned size = (type == 'd') ? 0 : fs::file_size(p);
		data = std::move(data).push_back(File(file_name, type, size));
	}
}

immer::flex_vector<File> Current_dir::ls() const
{
	return data;
}

Current_dir Current_dir::cd(const std::string& dir) const
{
	//FIX throw 
	fs::path dir_path = path / dir;
	if (!fs::is_directory(dir_path))
		throw "nod a dir";
	return Current_dir(dir_path);
}

void Current_dir::rename_on_system(const std::string& file_name, const std::string& new_file_name) const
{
	try { 
		// side effect?!
		fs::rename(path / file_name, path / new_file_name);
	}
	catch(const fs::filesystem_error& e) {
		// TODO -> what to do? 
		std::cerr << "Error while renaming the file:" << std::endl;
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

unsigned Current_dir::file_search(const std::string& file_name) const
{
	auto it = std::find_if(data.begin(), data.end(), [&](auto&& f) { return f.get_name() == file_name; });
	return std::distance(data.begin(), it);
}

Current_dir Current_dir::rename(const std::string& file_name, const std::string& new_file_name) const &
{
	rename_on_system(file_name, new_file_name);

	int index = file_search(file_name);

	return Current_dir(path, data.set(index, data[index].rename(new_file_name)));
}

Current_dir Current_dir::rename(const std::string& file_name, const std::string& new_file_name) &&
{
	rename_on_system(file_name, new_file_name);

	Current_dir d(std::forward<Current_dir>(*this));

	int index = file_search(file_name);

	d.data = std::move(d.data).set(index, data[index].rename(new_file_name));

	return d;
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
	if (file_search(f.get_name()) < data.size()) {
		std::cerr << "file already exists" << std::endl;
		return *this;
	}
	insert_on_system(f);
	auto new_data = data.push_back(std::forward<File>(f));
	return Current_dir(path, std::move(new_data));
}

Current_dir Current_dir::insert_file(File&& f) &&
{
	if (file_search(f.get_name()) < data.size()) {
		std::cerr << "file already exists" << std::endl;
		return *this;
	}
	insert_on_system(f);
	auto new_data = std::move(data).push_back(std::forward<File>(f));
	return Current_dir(path, std::move(new_data));
}

std::uintmax_t Current_dir::remove_from_system(const std::string& file_name) const
{
	return fs::remove_all(path / file_name);
}

Current_dir Current_dir::delete_file(const std::string& file_name) const &
{
	unsigned index = file_search(file_name);
	if (index < data.size()) {
		remove_from_system(file_name);
		return Current_dir(path, data.erase(index));
	}
	else
		return *this;
}

Current_dir Current_dir::delete_file(const std::string& file_name) &&
{
	unsigned index = file_search(file_name);
	if (index < data.size()) {
		remove_from_system(file_name);
		Current_dir dir(std::forward<Current_dir>(*this));
		dir.data = std::move(dir.data).erase(index);
		return dir;
	}
	else
		return *this;
}
