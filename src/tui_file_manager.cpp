#include "tui_file_manager.hpp"
#include "tui_fm_text_input.hpp"
#include "tui_fm_yes_no_menu_widget.hpp"
#include "system.hpp"
#include "projections.hpp"
#include <iostream>
#include <range/v3/view.hpp>
#include <range/v3/action/sort.hpp>


// exit application slot
sig::Slot<void()> exit_slot(file_manager_tui& fm) 
{
	sig::Slot<void()> slot{[&fm] {
		sig::Slot<void()> no_slot{[&fm] {
			fm.file_info.set_visible(true);
			fm.confirmation_widget.set_visible(false);
			Focus::set_focus_to(&fm.flisting);
			fm.update();
		}};

		fm.confirmation_widget.add_yes_no_slots("Do you really want to exit the program", System::quit, no_slot);
		fm.confirmation_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.confirmation_widget.set_visible(true);
		fm.update();
	}};

	slot.track(fm.destroyed);

	return slot;
}


// change directory slot
sig::Slot<void()> chdir(file_manager_tui& fm, const std::string& dirpath)
{
	sig::Slot<void()> slot{[&fm, dirpath] {
		auto new_dir = fm.curdir.cd(fs::absolute(dirpath));
		if (!new_dir.is_error_dir())
			fm.set_directory(new_dir, false, 0);
	}};

	slot.track(fm.destroyed);

	return slot;
}

// delete file slot
sig::Slot<void()> delete_file(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] {
		if (fm.flisting.size() == 0)
			return;


		std::size_t index = fm.flisting.selected_index();
		int new_index = index-1;
		auto file = fm.curdir.file_by_index(fm.offset + index).get();
		std::string file_name = file.name();

		// Yes slot
		sig::Slot<void()> yes_slot{[&fm, file, new_index] {
			if (new_index >= 0) {
				auto new_dir = fm.curdir.delete_file(file);

				if (!new_dir.is_error_dir()) {
					sys::remove_from_system(fm.curdir.path(file));
					fm.set_directory(new_dir, true, fm.offset);
					fm.flisting.select_item(new_index);
				}
			}
			else {
				if (fm.offset == 0) {
					auto new_dir = fm.curdir.delete_file(file);
					if (!new_dir.is_error_dir()) {
						sys::remove_from_system(fm.curdir.path(file));
						fm.set_directory(fm.curdir.delete_file(file), true, fm.offset);
						fm.flisting.select_item(0);
					}
				}
				else {
					auto new_dir = fm.curdir.delete_file(file);
					if (!new_dir.is_error_dir()) {
						sys::remove_from_system(fm.curdir.path(file));
						fm.set_directory(fm.curdir.delete_file(file), true, fm.offset-1);
						fm.flisting.select_item(fm.flisting.size()-1);
					}
				}
			}
			fm.file_info.set_visible(true);
			fm.confirmation_widget.set_visible(false);
			Focus::set_focus_to(&fm.flisting);
			fm.update();
		}};

		// No slot
		sig::Slot<void()> no_slot{[&fm] {
			fm.file_info.set_visible(true);
			fm.confirmation_widget.set_visible(false);
			Focus::set_focus_to(&fm.flisting);
			fm.update();
		}};

		fm.confirmation_widget.add_yes_no_slots("Do you really want to delete " + file_name, yes_slot, no_slot);
		fm.confirmation_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.confirmation_widget.set_visible(true);

		fm.update();
	}};

	slot.track(fm.destroyed);

	return slot;
}


sig::Slot<void()> insert_rfile(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		fm.insert_widget.change_title("New regular file");
		fm.insert_widget.change_info_message("Type name:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string&)> insert_rfile_slot{[&fm] (const std::string& text_new_name) {
			auto new_dir = fm.curdir.insert_file(file(text_new_name, REGULAR));

			if (!new_dir.is_error_dir()) {
				sys::insert_rfile_on_system(fm.curdir.path(text_new_name));

				int index = new_dir.file_index(text_new_name);
				auto height = fm.flisting.menu_height();

				fm.set_directory(new_dir, true, ((int)index/height)*height);

				if (index != -1)
					fm.flisting.select_item(index%height);
			}


			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			fm.insert_widget.editing_finished.disconnect_all_slots();
			fm.insert_widget.editing_canceled.disconnect_all_slots();

			fm.update();

		}};



		sig::Slot<void()> cancel_editing_slot{[&fm] () {
			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			fm.insert_widget.editing_finished.disconnect_all_slots();
			fm.insert_widget.editing_canceled.disconnect_all_slots();
			fm.update();
		}};


		fm.insert_widget.editing_finished.connect(insert_rfile_slot);
		fm.insert_widget.editing_canceled.connect(cancel_editing_slot);
		insert_rfile_slot.track(fm.insert_widget.destroyed);

	}};

	slot.track(fm.destroyed);

	return slot;
}


sig::Slot<void()> insert_dir(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		fm.insert_widget.change_title("New dir");
		fm.insert_widget.change_info_message("Type name:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string&)> insert_dir_slot{[&fm] (const std::string& text_new_name) {

			auto new_dir = fm.curdir.insert_file(file(text_new_name, DIRECTORY));

			if (!new_dir.is_error_dir()) {
				sys::insert_dir_on_system(fm.curdir.path(text_new_name));
				int index = new_dir.file_index(text_new_name);
				auto height = fm.flisting.menu_height();

				fm.set_directory(new_dir, true, ((int)index/height)*height);

				if (index != -1)
					fm.flisting.select_item(index%height);
			}

			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			fm.insert_widget.text_input.editing_finished.disconnect_all_slots();
			fm.update();
		}};

		sig::Slot<void()> cancel_editing_slot{[&fm] () {
			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			fm.insert_widget.editing_finished.disconnect_all_slots();
			fm.insert_widget.editing_canceled.disconnect_all_slots();
			fm.update();
		}};

		fm.insert_widget.text_input.editing_finished.connect(insert_dir_slot);
		fm.insert_widget.editing_canceled.connect(cancel_editing_slot);
		insert_dir_slot.track(fm.insert_widget.destroyed);
	}};

	slot.track(fm.destroyed);


	return slot;

}

sig::Slot<void()> change_file(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] 
		{
			fm.file_info.set_file(fm.curdir.file_by_index(fm.offset+fm.flisting.selected_index()).get());
		}};

	slot.track(fm.flisting.destroyed);

	return slot;
}

sig::Slot<void()> rename_selected(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {
		if (fm.flisting.size() == 0)
			return;

		auto selected_file = fm.curdir.file_by_index(fm.offset+fm.flisting.selected_index()).get();
		std::string file_name = selected_file.name();

		fm.insert_widget.change_title("Rename file " + file_name);
		fm.insert_widget.change_info_message("Type new name:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string &text)> rename_slot{[selected_file, &fm] (const std::string& text_new_name) {

			Focus::set_focus_to(&fm.flisting);

			auto new_dir = fm.curdir.rename(selected_file, text_new_name);

			if (!new_dir.is_error_dir()) {
				sys::rename_on_system(fm.curdir.path(selected_file), fm.curdir.path(text_new_name)); // important to be before set_directory so one can cd into renamed dir
				int index = new_dir.file_index(text_new_name);
				auto height = fm.flisting.menu_height();

				fm.set_directory(new_dir, true, ((int)index/height)*height);

				if (index != -1)
					fm.flisting.select_item(index%height);
			}
			else {
				// name exists
			}

			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);
			fm.insert_widget.text_input.editing_finished.disconnect_all_slots();
			fm.update();

		}};

		sig::Slot<void()> cancel_editing_slot{[&fm] () {
			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			fm.insert_widget.editing_finished.disconnect_all_slots();
			fm.insert_widget.editing_canceled.disconnect_all_slots();
			fm.update();
		}};

		fm.insert_widget.text_input.editing_finished.connect(rename_slot);
		fm.insert_widget.editing_canceled.connect(cancel_editing_slot);
		rename_slot.track(fm.insert_widget.destroyed);
	}};

	slot.track(fm.destroyed);

	return slot;
}

sig::Slot<void()> open_terminal(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		/* std::system(" echo $TERM | grep -o -e '[A-Za-z]\+' | head -n 1 > terminal_pluton"); */

		/* auto output =std::ifstream("terminal_pluton"); */
		/* std::string line; */


		/* if (std::getline(output, line)) { */
		std::stringstream ss;
		ss << "termite" << " -d " << fm.curdir.path().get() << " 2> /dev/null&";

		std::system(ss.str().c_str());
		/* } */

		/* fs::remove("terminal_pluton"); */
	}};

	slot.track(fm.destroyed);

	return slot;
}

sig::Slot<void()> history_undo(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {
		if (fm.history_index >= 1) {
			fm.set_directory(fm.dirs_history[fm.history_index-1], false, 0);
			fm.history_index--;

			/* fm.flisting.h_pressed.disable(); */
			fm.flisting.d_pressed.disable();
			fm.flisting.insert_rfile.disable();
			fm.flisting.insert_dir.disable();
			fm.flisting.rename_selected.disable();
			fm.flisting.run_file.disable();
			fm.flisting.terminal.disable();
		}
	}};

	slot.track(fm.destroyed);

	return slot;
}

sig::Slot<void()> history_redo(file_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {
		if ((int)fm.history_index < (int)fm.dirs_history.size()-1) {

			fm.history_index++;
			fm.set_directory(fm.dirs_history[fm.history_index], false, 0);

			/* fm.flisting.h_pressed.disable(); */
			fm.flisting.d_pressed.disable();
			fm.flisting.insert_rfile.disable();
			fm.flisting.insert_dir.disable();
			fm.flisting.rename_selected.disable();
			fm.flisting.run_file.disable();
			fm.flisting.terminal.disable();
		}
	}};

	slot.track(fm.destroyed);

	return slot;
}

sig::Slot<void()> exec_command(file_manager_tui& fm)
{

	sig::Slot<void()> slot{[&fm] () {

		fm.insert_widget.change_title("run file");
		fm.insert_widget.change_info_message("Type program:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string&)> insert_rfile_slot{[&fm] (const std::string& command) {

			auto selected_file = fm.curdir.file_by_index(fm.offset+fm.flisting.selected_index()).get();
			std::string file_name = selected_file.name();
			fs::path file_path = fm.curdir.path(file_name);
			std::stringstream ss1;

			ss1 <<"cat /usr/share/applications/"<<command<<".desktop 2> /dev/null | grep Terminal>result_pluton";
			std::system(ss1.str().c_str());

			auto output =std::ifstream("result_pluton");
			std::string line;


			if (std::getline(output, line)) {
				std::stringstream ss;
				if (line=="Terminal=true")
					ss << "termite -e " << "'" << command << " \"" << file_path.c_str() << "\"'&";
				else if (line=="Terminal=false")
					ss << command << " \"" << file_path.c_str() << "\" 2> /dev/null&";

				std::system(ss.str().c_str());
			}

			fs::remove("result_pluton");

			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			fm.insert_widget.editing_finished.disconnect_all_slots();
			fm.insert_widget.editing_canceled.disconnect_all_slots();

			fm.update();

		}};



		sig::Slot<void()> cancel_editing_slot{[&fm] () {
			fm.insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			fm.insert_widget.editing_finished.disconnect_all_slots();
			fm.insert_widget.editing_canceled.disconnect_all_slots();
			fm.update();
		}};


		fm.insert_widget.editing_finished.connect(insert_rfile_slot);
		fm.insert_widget.editing_canceled.connect(cancel_editing_slot);
		insert_rfile_slot.track(fm.insert_widget.destroyed);

	}};

	slot.track(fm.destroyed);
	return slot;
}

void file_manager_tui::set_items()
{
	auto height = flisting.menu_height();
	auto num_of_files = curdir.num_of_files();

	if (offset > num_of_files)
		offset = num_of_files > height ? num_of_files - height : 0; 

	if (offset < height)
		offset = 0;


	auto l = offset;
	auto r = std::min(offset + height, num_of_files);

	if (l == r)
		l = r - height;


	std::vector<std::tuple<file, opt::Optional<sig::Slot<void()>>>> menu_items;

	 std::transform(curdir.dirs(l), curdir.dirs(r), std::back_inserter(menu_items), [&](auto&& f) { return std::make_tuple(f, chdir(*this, curdir.path(f))); });

	 std::transform(curdir.regs(l), curdir.regs(r), std::back_inserter(menu_items), [](auto&& f) { return std::make_tuple(f, opt::none); });
	


	flisting.set_items(menu_items);
}

sig::Slot<void()> next_items(file_manager_tui &fm)
{
	sig::Slot<void()> slot{[&fm] {
		if (fm.offset + fm.flisting.menu_height() < fm.curdir.num_of_files() && fm.flisting.selected_index() == fm.flisting.size()-1) {
			fm.offset += fm.flisting.size(); 
			fm.set_items();
			// fix this -1
			fm.flisting.change_selected(-1);
		}
	}};

	slot.track(fm.destroyed);

	return slot;
}

sig::Slot<void()> back_items(file_manager_tui &fm)
{
	sig::Slot<void()> slot{[&fm] {
		if (fm.offset > 0 && fm.flisting.selected_index() == 0) {
			auto height = fm.flisting.menu_height();
			auto old_offset = fm.offset;
			fm.offset -= std::min(height, fm.offset); 
			auto old_selected_index = fm.flisting.selected_index();
			fm.set_items();
			fm.flisting.change_selected(fm.flisting.size() - old_selected_index);

			/* Resolves difference when menu height is larger than number of
			 * files in previous screen .. Instead of geting last of elements 
			 * (at index height) we reduce it by old_offset. */
			if (old_offset < height)
				fm.flisting.change_selected(fm.flisting.selected_index() - (height - old_offset));
		}
	}};

	slot.track(fm.destroyed);

	return slot;
}

static std::size_t old_height = 0;
static std::size_t old_width = 0;

sig::Slot<void(std::size_t, std::size_t)> reload_items(file_manager_tui &fm)
{
	sig::Slot<void(std::size_t, std::size_t)> slot{[&fm] (std::size_t width, std::size_t height) {
		if (height != 0 && width != 0 && (width != old_width || height != old_height)) {
			auto old_selected_index = fm.flisting.selected_index();
			auto old_offset = fm.offset;
			auto menu_height = fm.flisting.menu_height();

			fm.offset = ((fm.offset+old_selected_index)/menu_height) * menu_height;

			fm.set_items();

			fm.flisting.change_selected(old_selected_index-(fm.offset-old_offset));

			// Signal selected file has changed just in case
			fm.flisting.selected_file_changed();
		}
		old_height = height;
		old_width = width;
	}};


	slot.track(fm.destroyed);

	return slot;
}


file_manager_tui::file_manager_tui(current_dir& curdir)
	: curdir(curdir), offset(0), history_index(0), 
	titlebar{make_child<Titlebar>("  P  L  U  T  O  N      F  M")},
	bs_cur_dir_before{make_child<Blank_height>(2)},
	curdir_path_label{make_child<Label>("")},
	bs_cur_dir_after{make_child<Blank_height>(2)},
	hlayout_dir_finfo{make_child<Horizontal_layout>()},
	vlayout_left{hlayout_dir_finfo.make_child<Vertical_layout>()},
	vlayout_right{hlayout_dir_finfo.make_child<Vertical_layout>()},
	flisting{vlayout_left.make_child<fm_dirlist_menu>()},
	file_info{vlayout_right.make_child<fm_finfo>()},
	insert_widget{vlayout_right.make_child<fm_text_input_widget>("","")},
	confirmation_widget{vlayout_right.make_child<fm_yes_no_menu_widget>()}
{
	// path label style 
	set_background(curdir_path_label, Color::White);
	set_foreground(curdir_path_label, Color::Black);
	curdir_path_label.brush.add_attributes(Attribute::Bold);

	// don't show insert and confirmation box
	insert_widget.set_visible(false);
	confirmation_widget.set_visible(false);

	// flisting style
	enable_border(flisting);

	Focus::set_focus_to(&flisting);

	set_directory(curdir, false, 0);
}

void file_manager_tui::update_history(const current_dir& new_curdir)
{
	if (dirs_history.empty() || dirs_history.back().path().get() != new_curdir.path().get()) {
		dirs_history.push_back(curdir);
		history_index++;
	}

	dirs_history.push_back(new_curdir);
	history_index++;
}

void file_manager_tui::set_info()
{
	if (curdir.num_of_dirs() > 0) 
		file_info.set_file(curdir.dir_by_index(0).get());
	else if (curdir.num_of_regular_files() > 0) 
		file_info.set_file(curdir.regular_file_by_index(0).get());
	else 
		file_info.set_file("Empty directory", "", "");
}

void file_manager_tui::set_directory(const current_dir& new_curdir, bool save_previous, std::size_t offset)
{
	this->offset = offset;

	// save a previous curdir in a history vector
	if (save_previous)
		update_history(new_curdir);

	curdir = new_curdir;

	set_info();
	set_items();

	curdir_path_label.set_text("  Dir: " + curdir.path().get().string());

	flisting.select_item(0);

	connect_slots();
}

void file_manager_tui::connect_slots()
{
	flisting.selected_file_changed.connect(change_file(*this));	

	if (fs::exists(curdir.path().get().parent_path())) {
		flisting.h_pressed.disconnect_all_slots();
		flisting.h_pressed.connect(chdir(*this, fs::absolute(curdir.path().get().parent_path())));

		flisting.backspace_pressed.disconnect_all_slots();
		flisting.backspace_pressed.connect(chdir(*this, fs::absolute(curdir.path().get().parent_path())));
	}

	flisting.esc_pressed.disconnect_all_slots();
	flisting.esc_pressed.connect(exit_slot(*this));

	flisting.d_pressed.disconnect_all_slots();
	flisting.d_pressed.connect(delete_file(*this));

	flisting.insert_rfile.disconnect_all_slots();
	flisting.insert_rfile.connect(insert_rfile(*this));

	flisting.insert_dir.disconnect_all_slots();
	flisting.insert_dir.connect(insert_dir(*this));

	flisting.rename_selected.disconnect_all_slots();
	flisting.rename_selected.connect(rename_selected(*this));

	flisting.run_file.disconnect_all_slots();
	flisting.run_file.connect(exec_command(*this));

	flisting.terminal.disconnect_all_slots();
	flisting.terminal.connect(open_terminal(*this));

	flisting.undo.disconnect_all_slots();
	flisting.undo.connect(history_undo(*this));

	flisting.redo.disconnect_all_slots();
	flisting.redo.connect(history_redo(*this));

	flisting.items_begin_boundary.disconnect_all_slots();
	flisting.items_begin_boundary.connect(back_items(*this));

	flisting.items_end_boundary.disconnect_all_slots();
	flisting.items_end_boundary.connect(next_items(*this));

	flisting.resized.disconnect_all_slots();
	flisting.resized.connect(reload_items(*this));
}

