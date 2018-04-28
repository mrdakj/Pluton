#include <iostream>
#include "file.hpp"
#include "current_dir.hpp"
#include <immer/vector.hpp>
#include <immer/algorithm.hpp>
#include <algorithm>

int main()
{

	/* test_dir je direktorijum za testiranje */
	Current_dir d("test_dir");

	//Current_dir d2 = d.rename("test_subdir", "renamed_test_subdir");

	immer::for_each(d.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });
	std::cout  << std::endl;

	//immer::for_each(d2.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; });

	/* Current_dir d3 = d.cd("headers"); */

	/* std::cout  << std::endl; */
	/* immer::for_each(d3.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */

	return 0;
}
