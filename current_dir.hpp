#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <immer/vector.hpp>
#include <immer/algorithm.hpp>
#include <range/v3/all.hpp>
#include <algorithm>
#include "file.hpp"
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

class Current_dir {
private:
	std::string path;
	immer::vector<File> data;

public:
	Current_dir(const std::string& path, immer::vector<File> data)
		: path(path), data(std::move(data))
	{
	}

	Current_dir(const std::string& path)
		: path(path)
	{
		//FIX make more functional
		for (auto & p : fs::directory_iterator(path)) {
			std::string file_name = p.path().filename().string();
			char type = (fs::is_directory(p)) ? 'd' : 'r';
			unsigned size = (type == 'd') ? 0 : fs::file_size(p);
			data = std::move(data).push_back(File(file_name, type, size));
		}
	}

	immer::vector<File> ls() const
	{
		return data;
	}

	Current_dir cd(const std::string& dir) const
	{
		//FIX throw 
		if (!fs::is_directory(dir))
			throw "nod a dir";
		//FIX dont use /
		return Current_dir(path+"/"+dir);
	}
	
	Current_dir rename(const std::string& file_name, const std::string& new_file_name) const &
	{
		auto it = std::find_if(data.begin(), data.end(), [&](auto&& f) { return f.get_name() == file_name; });
		int index = std::distance(data.begin(), it);

		return Current_dir(path, data.set(index, data[index].rename(new_file_name)));
	}

	Current_dir rename(const std::string& file_name, const std::string& new_file_name) &&
	{
		Current_dir d(std::forward<Current_dir>(*this));

		auto it = std::find_if(data.begin(), data.end(), [&](auto&& f) { return f.get_name() == file_name; });
		int index = std::distance(data.begin(), it);

		d.data = std::move(d.data).set(index, data[index].rename(new_file_name));

		return d;
	}
};

#endif /* CURRENT_DIR_HPP */
