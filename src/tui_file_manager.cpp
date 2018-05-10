#include "../include/tui_file_manager.hpp"
#include "../include/tui_text_input.hpp"

using namespace cppurses;


/* Change directory slot */
sig::Slot<void()> chdir(File_manager_tui& fm, const std::string& dirpath)
{
    //sig::Slot<void()> slot{[&pb] { pb.clicked(); }};
    //slot.track(pb.destroyed);
    //return slot;

    sig::Slot<void()> slot{[&fm, dirpath] {
	    fm.set_directory(fm.curdir.cd(fs::absolute(dirpath)));
	    // exit(EXIT_FAILURE);
    }};

    slot.track(fm.destroyed);

    return slot;
}

sig::Slot<void()> delete_file(File_manager_tui& fm)
{
    sig::Slot<void()> slot{[&fm] {
		std::size_t index = fm.flisting.selected_index_;

	    fm.set_directory(fm.curdir.delete_file(fm.curdir.get_by_index(index)));

		if (index != 0)
			index--;
		fm.flisting.select_item(index);
    }};

    slot.track(fm.destroyed);

    return slot;
}

sig::Slot<void()> insert_rfile(File_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		auto &insert_widget = fm.vlayout_right.make_child<Fm_text_input_widget>("New regular file", "Type file name:");

		fm.file_info.set_visible(false);
		fm.update();


		sig::Slot<void(const std::string&)> insert_rfile_slot{[&fm, &insert_widget] (const std::string& text_new_name) {

		    fm.set_directory(fm.curdir.insert_file(File(text_new_name, 'r')));
			int index = fm.curdir.get_index_by_name(text_new_name);
			if (index != -1)
				fm.flisting.select_item(index);


			insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			insert_widget.editing_finished.disconnect_all_slots();
			insert_widget.editing_canceled.disconnect_all_slots();
			/* TODO Remove child not working */
			//fm.vlayout_right.remove_child(&insert_widget);
			fm.update();
			
		}};

		sig::Slot<void()> cancel_editing_slot{[&fm, &insert_widget] () {
			insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			insert_widget.editing_finished.disconnect_all_slots();
			insert_widget.editing_canceled.disconnect_all_slots();
			fm.update();
		}};


		insert_widget.editing_finished.connect(insert_rfile_slot);
		insert_widget.editing_canceled.connect(cancel_editing_slot);
		insert_rfile_slot.track(insert_widget.destroyed);

	}};

    slot.track(fm.destroyed);


    return slot;
}


sig::Slot<void()> insert_dir(File_manager_tui& fm)
{
	sig::Slot<void()> slot{[&fm] () {

		auto &insert_widget = fm.vlayout_right.make_child<Fm_text_input_widget>("New directory", "Type directory name:");

		fm.file_info.set_visible(false);
		fm.update();


		sig::Slot<void(const std::string&)> insert_dir_slot{[&fm, &insert_widget] (const std::string& text_new_name) {

		    fm.set_directory(fm.curdir.insert_file(File(text_new_name, 'd')));
			int index = fm.curdir.get_index_by_name(text_new_name);
			if (index != -1)
				fm.flisting.select_item(index);


			insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			insert_widget.text_input.editing_finished.disconnect_all_slots();
			/* TODO Remove child not working */
			//fm.vlayout_right.remove_child(&insert_widget);
			fm.update();
		}};

		sig::Slot<void()> cancel_editing_slot{[&fm, &insert_widget] () {
			insert_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			insert_widget.editing_finished.disconnect_all_slots();
			insert_widget.editing_canceled.disconnect_all_slots();
			fm.update();
		}};

		insert_widget.text_input.editing_finished.connect(insert_dir_slot);
		insert_widget.editing_canceled.connect(cancel_editing_slot);
		insert_dir_slot.track(insert_widget.destroyed);
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
		auto &rename_widget = fm.vlayout_right.make_child<Fm_text_input_widget>("Rename " + file_name, "Type new name:", file_name);

		fm.file_info.set_visible(false);
		fm.update();

		sig::Slot<void(const std::string &text)> rename_slot{[selected_file, &fm, &rename_widget] (const std::string& text_new_name) {

			Focus::set_focus_to(&fm.flisting);

       		        fm.set_directory(fm.curdir.rename(selected_file, text_new_name));

			int index = fm.curdir.get_index_by_name(text_new_name);
			if (index != -1)
				fm.flisting.select_item(index);
			
			rename_widget.set_visible(false);
			fm.file_info.set_visible(true);
			rename_widget.text_input.editing_finished.disconnect_all_slots();
			// TODO NOT WORKING -> REMOVE CHILD -> MEMORY LEAK ???
			//fm.vlayout_right.remove_child(&rename_widget);
			fm.update();

		}};

		sig::Slot<void()> cancel_editing_slot{[&fm, &rename_widget] () {
			rename_widget.set_visible(false);
			fm.file_info.set_visible(true);

			Focus::set_focus_to(&fm.flisting);
			rename_widget.editing_finished.disconnect_all_slots();
			rename_widget.editing_canceled.disconnect_all_slots();
			fm.update();
		}};

		rename_widget.text_input.editing_finished.connect(rename_slot);
		rename_widget.editing_canceled.connect(cancel_editing_slot);
		rename_slot.track(rename_widget.destroyed);
    }};

    slot.track(fm.destroyed);

    return slot;
}
 
File_manager_tui::File_manager_tui(Current_dir& curdir) : curdir(curdir)
{
	init(curdir);
}


void File_manager_tui::init(const Current_dir& curdir)
{
	set_background(*this, Color::Black);
	//hlLabel.set_alignment(Alignment::Center);
	set_background(current_dir_path, Color::White);
	set_foreground(current_dir_path, Color::Black);

	Focus::set_focus_to(&flisting);
	enable_border(flisting);

	
	current_dir_path.brush.add_attributes(Attribute::Bold);
	set_directory(curdir);
}

void File_manager_tui::set_directory(const Current_dir& new_curdir)
{
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

	flisting.esc_pressed.disconnect_all_slots();
	flisting.esc_pressed.connect(chdir(*this, fs::absolute(curdir.get_path().parent_path())));

	flisting.d_pressed.disconnect_all_slots();
	flisting.d_pressed.connect(delete_file(*this));


	flisting.insert_rfile.disconnect_all_slots();
	flisting.insert_rfile.connect(insert_rfile(*this));

	flisting.insert_dir.disconnect_all_slots();
	flisting.insert_dir.connect(insert_dir(*this));

	flisting.rename_selected.disconnect_all_slots();
	flisting.rename_selected.connect(rename_selected(*this));

	this->current_dir_path.set_text("  Dir: " + curdir.get_path().string());
}
