#include "tui_fm_dirlist_menu.hpp"

fm_dirlist_menu::fm_dirlist_menu()
{
	this->title.set_alignment(Alignment::Center);
    	this->title.brush.add_attributes(Attribute::Bold);
    	this->blank_after_title.background_tile = L'─';

	//std::size_t height_of_children = 0;
	//auto children = this->children();
	//std::for_each(children.cbegin(), children.cend(), [&height_of_children](const Widget *w) {
	//	height_of_children += w->height();
	//});
	//height_without_menu_items = height_of_children;
	//std::ofstream program_output("program_output.txt");
	//program_output << "Height without menu items: " << height_without_menu_items << std::endl;
}

bool fm_dirlist_menu::paint_event() {
	return fm_menu::paint_event();
}

bool fm_dirlist_menu::key_press_event(Key key, char symbol) {

   	if (key == Key::h) {
		h_pressed();
	} else if (key == Key::Backspace) {
		backspace_pressed();
	} else if (key == Key::d) {
		d_pressed();
	} else if (key == Key::q) {
		insert_rfile();
	} else if (key == Key::w) {
		insert_dir();
	} else if (key == Key::r) {
		rename_selected();
	} else if (key == Key::e) {
		run_file();
	} else if (key == Key::t) {
		terminal();
	} else if (key == Key::u) {
		undo();
	} else if (key == Key::p) {
		redo();
	} else if (key == Key::j) {
		if (get_selected_index() >= this->size()-1)
			items_end_boundary();

	} else if (key == Key::k) {
		if (get_selected_index() <= 0)
			items_begin_boundary();
	}

	return	fm_menu::key_press_event(key, symbol);
}

bool fm_dirlist_menu::mouse_press_event(Mouse_button button,
                             Point global,
                             Point local,
                             std::uint8_t device_id) {
	return fm_menu::mouse_move_event(button, global, local, device_id);
}

bool fm_dirlist_menu::mouse_press_event_filter(Event_handler* receiver,
                                    Mouse_button button,
                                    Point global,
                                    Point local,
                                    std::uint8_t device_id) {
	return fm_menu::mouse_press_event_filter(receiver, button, global, local, device_id);
}

std::size_t fm_dirlist_menu::get_menu_height()  const
{
	return height() - height_without_menu_items;
}
