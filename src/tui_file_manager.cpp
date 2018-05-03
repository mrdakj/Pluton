#include "../include/tui_file_manager.hpp"

using namespace cppurses;


/* Change directory slot */
sig::Slot<void()> chdir(File_manager_tui& fm, const std::string& dirname)
{
    //sig::Slot<void()> slot{[&pb] { pb.clicked(); }};
    //slot.track(pb.destroyed);
    //return slot;

    sig::Slot<void()> slot{[&fm, dirname] {
	    fm.set_directory(fm.curdir.cd(dirname));
	    // exit(EXIT_FAILURE);
    }};

    slot.track(fm.destroyed);

    return slot;
}

sig::Slot<void()> change_file(File_manager_tui& fm)
{
    sig::Slot<void()> slot{[&fm] 
    {
	    std::string file_name = fm.flisting.get_selected_item_name();
	    fm.file_info.set_file(fm.curdir.find_by_fname(file_name));
    }};

    slot.track(fm.flisting.destroyed);

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

void File_manager_tui::set_directory(const Current_dir& curdir)
{

	// Pokupljenja stara velicina
	size_t old_size = flisting.size();

	this->curdir = curdir;

	/* Postavljanje inicijalnog fajla na finfo */
	if (curdir.dirs.size() > 0) 
		file_info.set_file(curdir.dirs[0]);
	else if (curdir.regular_files.size() > 0) 
		file_info.set_file(curdir.regular_files[0]);	
	else 
		file_info.set_file("Empty directory", "", "");

	immer::for_each(curdir.dirs, [this](auto&& f) { 
		flisting.add_item(f.get_name()).connect(chdir(*this, f.get_name()));
	});

	immer::for_each(curdir.regular_files, [this](auto&& f) { 
		flisting.add_item(f.get_name());
	});

	// TODO -> Ovako je uradjeno jer mora prvo da se 
	// dodaju novi elementi, eleminisati nekako petlju
	for (std::size_t i = 0; i < old_size; i++)
	       flisting.remove_item(0);	

	flisting.selected_file_changed.connect(change_file(*this));	

	this->current_dir_path.set_text("  Dir: " + fs::absolute(curdir.get_path()).string());
}
