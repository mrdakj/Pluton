#ifndef FILE_HPP
#define FILE_HPP 

#include <string>

enum file_type { REGULAR = 'r', DIRECTORY = 'd' };

class file {
	private:
		std::string m_name;
		file_type m_type;
		std::size_t m_size;

	public:
		file(const std::string& name, file_type type = REGULAR, std::size_t size = 0);

		file rename(const std::string& new_name) &&;
		file rename(const std::string& new_name) const &;

		file_type type() const;
		std::size_t size() const;
		const std::string& name() const;
		const std::string info() const;

		bool is_dir() const;
		bool is_regular() const;
};


#endif /* FILE_HPP */
