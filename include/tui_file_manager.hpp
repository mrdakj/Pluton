#ifndef TUI_FILE_MANAGER
#define TUI_FILE_MANAGER

#include <cppurses/cppurses.hpp>
#include "current_dir.hpp"
#include "tui_fm_dirlist_menu.hpp"
#include "tui_fm_finfo.hpp"
#include "tui_fm_text_input.hpp"
#include "tui_fm_yes_no_menu_widget.hpp"
#include <vector>

using namespace cppurses;

class File_manager_tui : public Vertical_layout {

public:
	Current_dir& curdir;
	std::size_t offset;
	std::vector<Current_dir> dirs_history;
	std::size_t history_index;


	File_manager_tui(Current_dir& curdir);

	void set_directory(const Current_dir& new_curdir, bool save_previous, std::size_t offset);
	void set_items();

private:
	void update_history(const Current_dir& new_curdir);
	void set_info();
	void connect_slots();

private:
	// title - Pluton FM
	Titlebar& titlebar;

	// dir path
	Blank_height& bs_cur_dir_before;
	Label& curdir_path_label;
	Blank_height& bs_cur_dir_after;

	Horizontal_layout& hlayout_dir_finfo;
	Vertical_layout& vlayout_left;
	Vertical_layout& vlayout_right;


public:
	Fm_dirlist_menu& flisting;
	// show file info here
	Fm_finfo& file_info;
	// input box
	Fm_text_input_widget& insert_widget;
	// confirmation box
	Fm_yes_no_menu_widget& confirmation_widget;
};

// Slots related to File_manager_tui
sig::Slot<void()> chdir(File_manager_tui &fm, const std::string& dirname);
sig::Slot<void()> change_file(File_manager_tui &fm); 
sig::Slot<void()> insert_rfile(File_manager_tui& fm);
sig::Slot<void()> insert_dir(File_manager_tui& fm);
sig::Slot<void()> rename_selected(File_manager_tui& fm);
sig::Slot<void()> exec_command(File_manager_tui& fm);


#endif // TUI_FILE_MANAGER
