#include "../include/tui.hpp"

using namespace cppurses;
 
File_manager_tui::File_manager_tui(Current_dir& curdir) : curdir(curdir)
{
	init(curdir);
}

sig::Slot<void()> Slot_chdir(File_manager_tui* fm, const std::string& dirname)
{
    //sig::Slot<void()> slot{[&pb] { pb.clicked(); }};
    //slot.track(pb.destroyed);
    //return slot;

    sig::Slot<void()> slot{[fm, dirname] {
	    fm->set_directory(fm->curdir.cd(dirname));
	    // exit(EXIT_FAILURE);
    }};

    return slot;
}

void File_manager_tui::init(const Current_dir& curdir)
{
	set_background(*this, Color::Black);
	//hlLabel.set_alignment(Alignment::Center);
	set_background(current_dir_path, Color::White);
	set_foreground(current_dir_path, Color::Black);

	Focus::set_focus_to(&flisting);
	set_directory(curdir);
	
        enable_border(flisting);
}

void File_manager_tui::set_directory(const Current_dir& curdir)
{
	// Pokupljenja stara velicina
	size_t old_size = flisting.size();

	this->curdir = curdir;

	immer::for_each(curdir.dirs, [this](auto&& f) { 
		flisting.add_item(f.get_name()).connect(Slot_chdir(this, f.get_name()));
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
