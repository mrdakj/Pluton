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

class file_manager_tui : public Vertical_layout {
	public:
		current_dir& curdir;
		std::size_t offset;
		std::vector<current_dir> dirs_history;
		std::size_t history_index;


		file_manager_tui(current_dir& curdir);

		void set_directory(const current_dir& new_curdir, bool save_previous, std::size_t offset);
		void set_items();

	private:
		void update_history(const current_dir& new_curdir);
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
		fm_dirlist_menu& flisting;
		// show file info here
		fm_finfo& file_info;
		// input box
		fm_text_input_widget& insert_widget;
		// confirmation box
		fm_yes_no_menu_widget& confirmation_widget;
};

// Slots related to file_manager_tui
sig::Slot<void()> chdir(file_manager_tui &fm, const std::string& dirname);
sig::Slot<void()> change_file(file_manager_tui &fm); 
sig::Slot<void()> insert_rfile(file_manager_tui& fm);
sig::Slot<void()> insert_dir(file_manager_tui& fm);
sig::Slot<void()> rename_selected(file_manager_tui& fm);
sig::Slot<void()> exec_command(file_manager_tui& fm);


#endif // TUI_FILE_MANAGER
