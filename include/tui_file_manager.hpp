#ifndef TUI_FILE_MANAGER
#define TUI_FILE_MANAGER

#include <cppurses/cppurses.hpp>
#include "current_dir.hpp"
#include "tui_fm_dirlist_menu.hpp"
#include "tui_fm_finfo.hpp"

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

	Horizontal_layout& hlayout_dir_finfo{this->make_child<Horizontal_layout>()};
	Fm_dirlist_menu& flisting{hlayout_dir_finfo.make_child<Fm_dirlist_menu>("Directory listing")};
	Fm_finfo& file_info{hlayout_dir_finfo.make_child<Fm_finfo>()};

};

	/* Sloto's related to File_manager_tui */
	sig::Slot<void()> chdir(File_manager_tui &fm, const std::string& dirname);
	sig::Slot<void()> change_file(File_manager_tui &fm); 

#endif // TUI_FILE_MANAGER
