#include <cppurses/cppurses.hpp>
#include "current_dir.hpp"

#ifndef TUI
#define TUI

using namespace cppurses;

/* Ovde idu podstvari u fm-u (save_area, italic-bold ...) */


class File_manager_tui : public Vertical_layout {

private:
	void init(const Current_dir& curdir);

public:
	Current_dir& curdir;

	File_manager_tui(Current_dir& curdir);

	void set_directory(const Current_dir& curdir);

	Titlebar& titlebar{this->make_child<Titlebar>("  P  L  U  T  O  N      F  M")};

	Blank_height& bs_cur_dir_before{this->make_child<Blank_height>(2)};
	Label& current_dir_path{this->make_child<Label>("")};
	Blank_height& bs_cur_dir_after{this->make_child<Blank_height>(2)};

	Menu& flisting{this->make_child<Menu>("Directory listing")};

};


sig::Slot<void()> Slot_chdir(File_manager_tui* fm, const std::string& dirname);

#endif // TUI
