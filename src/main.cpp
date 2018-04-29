#include <immer/vector.hpp>
#include <immer/algorithm.hpp>
#include <algorithm>
#include <iostream>
#include "../include/file.hpp"
#include "../include/current_dir.hpp"

int main()
{

	// test_dir is dir for testing
	Current_dir d("test_dir");

	// test_dir/test_subdir -> test_dir/renamed_test_subdir
	Current_dir d2 = d.rename("test_subdir", "renamed_test_subdir");

	// test_dir/renamed_test_subdir -> test_dir/test_subdir
	Current_dir d3 = d2.rename("renamed_test_subdir", "test_subdir");

	// cd into dir
	Current_dir d4 = d.cd("test_subdir");

	// insert new file/dir
	Current_dir d5 = d4.insert_file(File("new_dir", 'd'));

	// delete file/dir
	Current_dir d6 = d5.delete_file("new_dir");

	immer::for_each(d.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });
	std::cout  << std::endl;

	immer::for_each(d2.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });
	std::cout  << std::endl;

	immer::for_each(d3.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });
	std::cout  << std::endl;

	immer::for_each(d4.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });
	std::cout  << std::endl;

	immer::for_each(d5.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });
	std::cout  << std::endl;

	immer::for_each(d6.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });
	return 0;
}
