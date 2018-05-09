#include "tui_text_input.hpp"


bool Fm_text_input_widget::Text_input::key_press_event(Key key, char symbol)
{
    /* Extend key press-es here */
	

    /* Do the standard procedure of Line edit */
    return Line_edit::key_press_event(key, symbol);
}


bool Fm_text_input_widget::Text_input::focus_in_event() 
{

	/* Bypass focus_in of text input 
	 * wich clears the placeholder */

	return Textbox::focus_in_event();
}

Fm_text_input_widget::Text_input::Text_input(const Glyph_string& initial_text) : Line_edit(initial_text) {}


Fm_text_input_widget::Fm_text_input_widget(const Glyph_string &title, const Glyph_string &info_message, const Glyph_string &initial_text) : 
	title(this->make_child<Label>(title)), info_message(this->make_child<Label>(info_message)), text_input(this->make_child<Text_input>(initial_text))
{
	Focus::set_focus_to(&text_input);
	//lbl.set_text(initial_text);
	//ti.set_text(initial_text);
	
	this->title.set_alignment(Alignment::Center);
	this->title.brush.add_attributes(Attribute::Bold);
	this->title_blank.background_tile = L'─';
	enable_border(*this);
}

