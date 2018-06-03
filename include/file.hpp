#ifndef FILE_HPP
#define FILE_HPP 

#include <string>

enum file_type { REGULAR = 'r', DIRECTORY = 'd' };

class file {
private:
	std::string name;
	file_type type;
	std::size_t size;

public:
	file(const std::string& name, file_type type = REGULAR, std::size_t size = 0);

	file rename(const std::string& new_name) &&;
	file rename(const std::string& new_name) const &;

	file_type get_type() const;
	std::size_t get_size() const;
	const std::string& get_name() const;
	const std::string get_info() const;

	bool is_dir() const;
	bool is_regular() const;
};


#endif /* FILE_HPP */
