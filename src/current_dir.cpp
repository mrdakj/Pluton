#include "current_dir.hpp"
#include "file.hpp"

namespace fs = std::experimental::filesystem;

Current_dir::Current_dir(const std::string& path, immer::vector<File> data)
	: path(path), data(std::move(data))
{
	/* TODO FIX proveru greske ako nije dir dobar */
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

	/* TODO FIX proveru greske ako nije dir dobar */
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
		data = std::move(data).push_back(File(this, file_name, type, size));
	}
}

immer::vector<File> Current_dir::ls() const
{
	return data;
}

Current_dir Current_dir::cd(const std::string& dir) const
{
	//FIX throw 
	if (!fs::is_directory(dir))
		throw "nod a dir";
	// operator / makes path with '/'
	return Current_dir(path / dir);
}

Current_dir Current_dir::rename(const std::string& file_name, const std::string& new_file_name) const &
{
	auto it = std::find_if(data.begin(), data.end(), [&](auto&& f) { return f.get_name() == file_name; });
	int index = std::distance(data.begin(), it);

	return Current_dir(path, data.set(index, data[index].rename(new_file_name)));
}

Current_dir Current_dir::rename(const std::string& file_name, const std::string& new_file_name) &&
{
	Current_dir d(std::forward<Current_dir>(*this));

	auto it = std::find_if(data.begin(), data.end(), [&](auto&& f) { return f.get_name() == file_name; });
	int index = std::distance(data.begin(), it);

	d.data = std::move(d.data).set(index, data[index].rename(new_file_name));

	return d;
}

fs::path Current_dir::getPath() {
	return path;
}
