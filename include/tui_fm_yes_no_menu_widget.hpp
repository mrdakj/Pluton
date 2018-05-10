#ifndef TUI_FM_YES_NO_MENU_WIDGET
#define TUI_FM_YES_NO_MENU_WIDGET

#include "tui_fm_menu.hpp"

using namespace cppurses;

class Fm_yes_no_menu_widget : public Vertical_layout {
   public:
	
	Fm_yes_no_menu_widget(std::string question, sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot);

	Label& title{this->make_child<Label>()};
	Blank_height& title_blank{this->make_child<Blank_height>(1)};

	Label& question{this->make_child<Label>()};
	Blank_height& blank_after_question{this->make_child<Blank_height>(1)};

	Fm_menu& options_menu{this->make_child<Fm_menu>("")};

};

#endif  // TUI_YES_NO_MENU_WIDGET
