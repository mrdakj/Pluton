#ifndef FILE_HPP
#define FILE_HPP 

#include <string>

enum File_type { REGULAR = 'r', DIRECTORY = 'd' };

class File {
private:
	std::string name;
	File_type type;
	std::size_t size;

public:
	File(const std::string& name, File_type type = REGULAR, std::size_t size = 0);

	File rename(const std::string& new_name) &&;
	File rename(const std::string& new_name) const &;

	File_type get_type() const;
	std::size_t get_size() const;
	const std::string& get_name() const;
	const std::string get_info() const;

	bool is_dir() const;
	bool is_regular() const;
};

#endif /* FILE_HPP */
