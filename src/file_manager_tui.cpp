#include "../include/file_manager_tui.hpp"

using namespace cppurses;
 
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

	this->current_dir_path.set_text("  Directory: " + fs::absolute(curdir.get_path()).string());
}

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

    return slot;
}
