#include "../include/tui_file_manager.hpp"
#include "../include/tui_fm_text_input.hpp"
#include "../include/tui_fm_yes_no_menu_widget.hpp"
#include <unistd.h>


using namespace cppurses;


sig::Slot<void()> exit_slot(File_manager_tui& fm) 
{
    sig::Slot<void()> slot{[&fm] {
		sig::Slot<void()> no_slot{[&fm] {
			fm.file_info.set_visible(true);
			fm.confirmation_widget.set_visible(false);
			Focus::set_focus_to(&fm.flisting);
			fm.update();
		}};

		fm.confirmation_widget.add_yes_no_slots("Do you really want to exit program", System::quit, no_slot);
		fm.confirmation_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.confirmation_widget.set_visible(true);
		fm.update();
    }};

    slot.track(fm.destroyed);

    return slot;
}

/* Change directory slot */
sig::Slot<void()> chdir(File_manager_tui& fm, const std::string& dirpath)
{
    //sig::Slot<void()> slot{[&pb] { pb.clicked(); }};
    //slot.track(pb.destroyed);
    //return slot;

    sig::Slot<void()> slot{[&fm, dirpath] {
	    fm.set_directory(fm.curdir.cd(fs::absolute(dirpath)), false);
	    // exit(EXIT_FAILURE);
    }};

    slot.track(fm.destroyed);

    return slot;
}

sig::Slot<void()> delete_file(File_manager_tui& fm)
{
    sig::Slot<void()> slot{[&fm] {


		std::size_t index = fm.flisting.selected_index_;
		std::size_t new_index = (index != 0) ? index-1 : index;
		auto file = fm.curdir.get_by_index(index);
		std::string file_name = file.get_name();

	    	// Yes slot
		sig::Slot<void()> yes_slot{[&fm, file, new_index] {
		   	fm.set_directory(fm.curdir.delete_file(file), true);
			fm.flisting.select_item(new_index);
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


sig::Slot<void()> insert_rfile(File_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		fm.insert_widget.change_title("New regular file");
		fm.insert_widget.change_info_message("Type name:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string&)> insert_rfile_slot{[&fm] (const std::string& text_new_name) {

		    fm.set_directory(fm.curdir.insert_file(File(text_new_name, 'r')), true);

			int index = fm.curdir.get_index_by_name(text_new_name);
			if (index != -1)
				fm.flisting.select_item(index);


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


sig::Slot<void()> insert_dir(File_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		fm.insert_widget.change_title("New dir");
		fm.insert_widget.change_info_message("Type name:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string&)> insert_dir_slot{[&fm] (const std::string& text_new_name) {

		    fm.set_directory(fm.curdir.insert_file(File(text_new_name, 'd')), true);
			int index = fm.curdir.get_index_by_name(text_new_name);
			if (index != -1)
				fm.flisting.select_item(index);


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

sig::Slot<void()> change_file(File_manager_tui& fm)
{
    sig::Slot<void()> slot{[&fm] 
    {
		fm.file_info.set_file(fm.curdir.get_by_index(fm.flisting.selected_index_));
    }};

    slot.track(fm.flisting.destroyed);

    return slot;
}

sig::Slot<void()> rename_selected(File_manager_tui& fm)
{
    sig::Slot<void()> slot{[&fm] () {

		auto selected_file = fm.curdir.get_by_index(fm.flisting.selected_index_);
		std::string file_name = selected_file.get_name();

		fm.insert_widget.change_title("Rename file " + file_name);
		fm.insert_widget.change_info_message("Type new name:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string &text)> rename_slot{[selected_file, &fm] (const std::string& text_new_name) {

			Focus::set_focus_to(&fm.flisting);

       		        fm.set_directory(fm.curdir.rename(selected_file, text_new_name), true);

			int index = fm.curdir.get_index_by_name(text_new_name);
			if (index != -1)
				fm.flisting.select_item(index);
			
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

sig::Slot<void()> open_terminal(File_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		/* std::system(" echo $TERM | grep -o -e '[A-Za-z]\+' | head -n 1 > terminal_pluton"); */

		/* auto output =std::ifstream("terminal_pluton"); */
		/* std::string line; */


		/* if (std::getline(output, line)) { */
			std::stringstream ss;
			ss << "termite" << " -d " << fm.curdir.get_path() << " 2> /dev/null&";

			std::system(ss.str().c_str());
		/* } */

		/* fs::remove("terminal_pluton"); */
	}};

    slot.track(fm.destroyed);

    return slot;
}

sig::Slot<void()> history_undo(File_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {
		if (fm.history_index >= 1) {
				fm.flisting.clear();


				immer::for_each(fm.dirs_history[fm.history_index-1].ls(), [&fm](auto&& f) { 
						fm.flisting.add_item(f.get_name());
				});
				fm.history_index--;

				fm.flisting.selected_file_changed.disable();	
				fm.flisting.h_pressed.disable();
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

sig::Slot<void()> history_redo(File_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {
		if ((int)fm.history_index < (int)fm.dirs_history.size()-1) {
				fm.flisting.clear();

				fm.history_index++;

				immer::for_each(fm.dirs_history[fm.history_index].ls(), [&fm](auto&& f) { 
						fm.flisting.add_item(f.get_name());
						});

				fm.flisting.select_item(0);

				fm.flisting.selected_file_changed.disable();	
				fm.flisting.h_pressed.disable();
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

sig::Slot<void()> exec_command(File_manager_tui& fm)
{

	sig::Slot<void()> slot{[&fm] () {

		fm.insert_widget.change_title("run file");
		fm.insert_widget.change_info_message("Type program:");
		fm.insert_widget.grab_focus();

		fm.file_info.set_visible(false);
		fm.insert_widget.set_visible(true);
		fm.update();


		sig::Slot<void(const std::string&)> insert_rfile_slot{[&fm] (const std::string& command) {

			auto selected_file = fm.curdir.get_by_index(fm.flisting.selected_index_);
			std::string file_name = selected_file.get_name();
			fs::path file_path = fm.curdir.get_path() / file_name;
			std::stringstream ss1;

			ss1 <<"cat /usr/share/applications/"<<command<<".desktop 2> /dev/null | grep Terminal>result_pluton";
			std::system(ss1.str().c_str());

			auto output =std::ifstream("result_pluton");
			std::string line;


			if (std::getline(output, line)) {
				std::stringstream ss;
				if (line=="Terminal=true")
					ss << "termite -e " << "'" << command << " " << file_path.c_str() << "'&";
				else if (line=="Terminal=false")
					ss << command << " " << file_path.c_str() << " 2> /dev/null&";

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
 
File_manager_tui::File_manager_tui(Current_dir& curdir) : curdir(curdir), history_index(0)
{
	init(curdir);
}


void File_manager_tui::init(const Current_dir& curdir)
{
	set_background(*this, Color::Black);
	//hlLabel.set_alignment(Alignment::Center);
	set_background(current_dir_path, Color::White);
	set_foreground(current_dir_path, Color::Black);

	insert_widget.set_visible(false);
	confirmation_widget.set_visible(false);
	Focus::set_focus_to(&flisting);
	enable_border(flisting);

	
	current_dir_path.brush.add_attributes(Attribute::Bold);
	set_directory(curdir, false);
}

void File_manager_tui::set_directory(const Current_dir& new_curdir, bool ind)
{

	if (ind) {
		if (dirs_history.empty() || dirs_history[dirs_history.size()-1].get_path() != new_curdir.get_path()) {
			dirs_history.push_back(curdir);
			history_index++;
		}

		dirs_history.push_back(new_curdir);
		history_index++;

/* 		std::ofstream x("rez"); */
/* 		for (auto i : dirs_history) { */
/* 			for (auto j : i.ls()) { */
/* 				x << j.get_name() << std::endl; */
/* 			} */
/* 			x << std::endl; */
/* 		} */
	}

	curdir = new_curdir;

	// set info
	if (curdir.dirs.size() > 0) 
		file_info.set_file(curdir.dirs[0]);
	else if (curdir.regular_files.size() > 0) 
		file_info.set_file(curdir.regular_files[0]);	
	else 
		file_info.set_file("Empty directory", "", "");

	flisting.clear();

	immer::for_each(curdir.dirs, [this](auto&& f) { 
		flisting.add_item(f.get_name()).connect(chdir(*this, this->curdir.path / f.get_name()));
	});

	immer::for_each(curdir.regular_files, [this](auto&& f) { 
		flisting.add_item(f.get_name());
	});


	flisting.select_item(0);
	flisting.selected_file_changed.connect(change_file(*this));	

	flisting.h_pressed.disconnect_all_slots();
	flisting.h_pressed.connect(chdir(*this, fs::absolute(curdir.get_path().parent_path())));

	flisting.backspace_pressed.disconnect_all_slots();
	flisting.backspace_pressed.connect(chdir(*this, fs::absolute(curdir.get_path().parent_path())));

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

	current_dir_path.set_text("  Dir: " + curdir.get_path().string());
}
