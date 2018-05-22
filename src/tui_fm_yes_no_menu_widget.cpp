#include "tui_fm_yes_no_menu_widget.hpp"

Fm_yes_no_menu_widget::Fm_yes_no_menu_widget(std::string question, sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot)  
{
	//this->title.set_alignment(Alignment::Center);
    	//this->title.brush.add_attributes(Attribute::Bold);
    	this->blank_after_question_second.background_tile = L'─';
	enable_border(*this);

	this->question.set_text(question);
	options_menu.add_item("Yes").connect(yes_slot);
	options_menu.add_item("No").connect(no_slot);
	options_menu.esc_pressed.connect(no_slot);
}

Fm_yes_no_menu_widget::Fm_yes_no_menu_widget()  
{
    	this->blank_after_question_second.background_tile = L'─';
	enable_border(*this);
}

void Fm_yes_no_menu_widget::grab_focus()
{
	Focus::set_focus_to(&options_menu);
}

void Fm_yes_no_menu_widget::add_yes_no_slots(sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot)
{
	options_menu.remove_item(0);
	options_menu.remove_item(0);
	options_menu.add_item("Yes").connect(yes_slot);
	options_menu.add_item("No").connect(no_slot);
	options_menu.esc_pressed.connect(no_slot);
}

void Fm_yes_no_menu_widget::add_yes_no_slots(std::string question, sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot)
{
	this->question.set_text(question);
	add_yes_no_slots(yes_slot, no_slot);
}
