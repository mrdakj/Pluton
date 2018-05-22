#include <immer/vector.hpp>
#include <immer/algorithm.hpp>
#include <algorithm>
#include <iostream>
#include "file.hpp"
#include "current_dir.hpp"
#include "tui_file_manager.hpp"
#include <cppurses/cppurses.hpp>


int main()
{
	// test_dir is dir for testing
	/* Current_dir d("/home/jelena/GitHub"); */
	Current_dir d(fs::absolute(""));
	/* // test_dir/test_subdir -> test_dir/renamed_test_subdir */
	/* Current_dir d2 = d.rename(File("test_subdir", 'd'), "renamed_test_subdir"); */

	/* // test_dir/renamed_test_subdir -> test_dir/test_subdir */
	/* Current_dir d3 = d2.rename(File("renamed_test_subdir", 'd'), "test_subdir"); */

	/* // cd into dir */
	/* Current_dir d4 = d.cd(File("test_subdir")); */

	/* // insert new file/dir */
	/* Current_dir d5 = d4.insert_file(File("new_dir", 'd')); */

	/* // delete file/dir */
	/* Current_dir d6 = d5.delete_file(File("new_dir",'d')); */

	/* // Root path */
	/* /1* Current_dir d7 = d.cd("test_subdir"); // this doesnt work *1/ */

	/* std::cout << "init Current_dir with test_dir" << std::endl; */
	/* immer::for_each(d.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */
	/* std::cout  << std::endl; */

	/* std::cout << "rename test_subdir to renamed_test_subdir" << std::endl; */
	/* immer::for_each(d2.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */
	/* std::cout  << std::endl; */

	/* std::cout << "rename  renamed_test_subdir to test_subdir" << std::endl; */
	/* immer::for_each(d3.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */
	/* std::cout  << std::endl; */

	/* std::cout << "cd in test_subdir" << std::endl; */
	/* immer::for_each(d4.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */
	/* std::cout  << std::endl; */

	/* std::cout << "make new_dir in test_subdir" << std::endl; */
	/* immer::for_each(d5.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */
	/* std::cout  << std::endl; */

	/* std::cout << "delete new_dir" << std::endl; */
	/* immer::for_each(d6.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */

	/* std::cout  << std::endl; */

	/* immer::for_each(d7.ls(), [](auto&& s) { std::cout << s.get_info() << std::endl; }); */

	File_manager_tui fm(d);	


	cppurses::System sys;
	sys.set_head(&fm);

	return sys.run();
}
