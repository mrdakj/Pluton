#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <immer/flex_vector.hpp>
#include <immer/algorithm.hpp>
#include <algorithm>
#include <experimental/filesystem>
#include "file.hpp"
#include <fstream>
#include <iostream>


namespace fs = std::experimental::filesystem;

template <class Compare>
class Current_dir {
private:
	Compare comp;

	fs::path path;
	immer::flex_vector<File> data;

	void rename_on_system(const File& f, const std::string& new_file_name) const;

	void insert_on_system(const File& f) const;
	std::uintmax_t remove_from_system(const File& f) const;
	unsigned file_search(const File& f) const;
	unsigned place_to_insert(const File& f) const;

public:

	Current_dir(const std::string& path, immer::flex_vector<File> data, const Compare& comp = Compare());
	Current_dir(const std::string& path, const Compare& comp = Compare());

	immer::flex_vector<File> ls() const;

	Current_dir cd(const std::string& dir) const;

	Current_dir rename(const File& f, const std::string& new_file_name) const &;
	Current_dir rename(const File& f, const std::string& new_file_name) &&;

	Current_dir insert_file(File&& f) const &;
	Current_dir insert_file(File&& f) &&;


	Current_dir delete_file(const File& f) const &;
	Current_dir delete_file(const File& f) &&;

	const fs::path& get_path() const;
};

template <class Compare>
Current_dir<Compare>::Current_dir(const std::string& path, immer::flex_vector<File> data, const Compare& comp)
	: comp(comp), path(path), data(std::move(data))
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

template <class Compare>
Current_dir<Compare>::Current_dir(const std::string& path, const Compare& comp)
	: comp(comp), path(path)
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
		File f(file_name, type, size);
		int index = place_to_insert(f);
		data = std::move(data).insert(index,std::move(f));
	}
}

template <class Compare>
immer::flex_vector<File> Current_dir<Compare>::ls() const
{
	return data;
}

template <class Compare>
Current_dir<Compare> Current_dir<Compare>::cd(const std::string& dir) const
{
	//FIX throw 
	fs::path dir_path = path / dir;
	if (!fs::is_directory(dir_path))
		throw "nod a dir";
	return Current_dir(dir_path);
}

template <class Compare>
void Current_dir<Compare>::rename_on_system(const File& f, const std::string& new_file_name) const
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

template<class ForwardIt, class T, class Compare=std::less<>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp={})
{
    first = std::lower_bound(first, last, value, comp);
    return first != last && !comp(value, *first) ? first : last;
}

template <class Compare>
unsigned Current_dir<Compare>::file_search(const File& f) const
{
	auto it = binary_find(data.begin(), data.end(), f, comp);

    return std::distance(data.begin(), it);
}

template <class Compare>
unsigned Current_dir<Compare>::place_to_insert(const File& f) const
{
	auto it = std::lower_bound(data.begin(), data.end(), f, comp);

    return std::distance(data.begin(), it);
}

template <class Compare>
Current_dir<Compare> Current_dir<Compare>::rename(const File& f, const std::string& new_file_name) const &
{
	rename_on_system(f, new_file_name);

	int index = file_search(f);
	auto new_data = data.erase(index);
	int new_place = place_to_insert(File(new_file_name, f.get_type(), f.get_size()));
	new_data = std::move(new_data).insert(new_place, f.rename(new_file_name));

	return Current_dir(path, std::move(new_data));
}

template <class Compare>
Current_dir<Compare> Current_dir<Compare>::rename(const File& f, const std::string& new_file_name) &&
{
	rename_on_system(f, new_file_name);
	int index = file_search(f);
	auto new_data = std::move(data).erase(index);
	int new_place = place_to_insert(File(new_file_name, f.get_type(), f.get_size()));
	new_data = std::move(new_data).insert(new_place, f.rename(new_file_name));

	return Current_dir(path, std::move(new_data));
}

template <class Compare>
const fs::path& Current_dir<Compare>::get_path() const
{
	return path;
}


template <class Compare>
void Current_dir<Compare>::insert_on_system(const File& f) const
{
	// TODO check errors and check if a file already exists
	if (f.get_type() == 'd')
		fs::create_directory(path / f.get_name());
	if (f.get_type() == 'r')
		std::ofstream(path / f.get_name());
}

template <class Compare>
Current_dir<Compare> Current_dir<Compare>::insert_file(File&& f) const &
{
	insert_on_system(f);
	int place = place_to_insert(f);

	// Check if file with same name already exist in data vector 
	// True -> Don't do any modifications and return same Current_dir object
	// [TODO] When vector length is > 1 and file exist in folder -> index of file is at place + 1,
	// [TODO] When vector length is == 1 and file exist in folder -> index of file is at place,
	// [TODO] Because of that, there is need for double check ' || '
	if (data[place].get_name() == f.get_name() || data[place + 1].get_name() == f.get_name())
		return *this;

	// False -> return new Current_dir object
	auto new_data = data.insert(place, std::forward<File>(f));
	return Current_dir(path, std::move(new_data));
}

template <class Compare>
Current_dir<Compare> Current_dir<Compare>::insert_file(File&& f) &&
{
	insert_on_system(f);
	int place = place_to_insert(f);

	// Check if file with same name already exist in data vector 
	// True -> Don't do any modifications and return same Current_dir object
	// [TODO] When vector length is > 1 and file exist in folder -> index of file is at place + 1,
	// [TODO] When vector length is == 1 and file exist in folder -> index of file is at place,
	// [TODO] Because of that, there is need for double check ' || '
	if (data[place].get_name() == f.get_name() || data[place + 1].get_name() == f.get_name())
		return *this;

	// False -> return new Current_dir object
	auto new_data = std::move(data).insert(place, std::forward<File>(f));
	return Current_dir(path, std::move(new_data));
}

template <class Compare>
std::uintmax_t Current_dir<Compare>::remove_from_system(const File& f) const
{
	return fs::remove_all(path / f.get_name());
}

template <class Compare>
Current_dir<Compare> Current_dir<Compare>::delete_file(const File& f) const &
{
	unsigned index = file_search(f);
	if (index < data.size()) {
		remove_from_system(f);
		return Current_dir(path, data.erase(index));
	}
	else
		return *this;
}

template <class Compare>
Current_dir<Compare> Current_dir<Compare>::delete_file(const File& f) &&
{
	unsigned index = file_search(f);
	if (index < data.size()) {
		remove_from_system(f);
		Current_dir dir(std::forward<Current_dir>(*this));
		dir.data = std::move(dir.data).erase(index);
		return dir;
	}
	else
		return *this;
}

#endif /* CURRENT_DIR_HPP */
