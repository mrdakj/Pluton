#include "../include/tui_fm_yes_no_menu_widget.hpp"

Fm_yes_no_menu_widget::Fm_yes_no_menu_widget(std::string question, sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot)  
{
	//this->title.set_alignment(Alignment::Center);
    	//this->title.brush.add_attributes(Attribute::Bold);
    	//this->title_blank.background_tile = L'─';
	enable_border(*this);

	this->question.set_text(question);
	this->options_menu.add_item("Yes").connect(yes_slot);
	this->options_menu.add_item("No").connect(no_slot);
	this->options_menu.esc_pressed.connect(no_slot);
}
