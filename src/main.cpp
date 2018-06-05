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
	current_dir d(fs::absolute(""));

	file_manager_tui fm(d);	

	cppurses::System sys;
	sys.set_head(&fm);

	return sys.run();
}
