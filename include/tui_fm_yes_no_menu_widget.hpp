#ifndef TUI_FM_YES_NO_MENU_WIDGET
#define TUI_FM_YES_NO_MENU_WIDGET

#include "tui_fm_menu.hpp"
#include <cppurses/cppurses.hpp>

using namespace cppurses;

class fm_yes_no_menu_widget : public Vertical_layout {
	public:
		fm_yes_no_menu_widget(std::string question, sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot);
		fm_yes_no_menu_widget();
		void add_yes_no_slots(sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot);
		void add_yes_no_slots(std::string question, sig::Slot<void()> yes_slot, sig::Slot<void()> no_slot);
		void grab_focus();


		Blank_height& blank_before_question{make_child<Blank_height>(1)};
		Label& question{make_child<Label>()};
		Blank_height& blank_after_question_first{make_child<Blank_height>(1)};
		Blank_height& blank_after_question_second{make_child<Blank_height>(1)};

		fm_menu& options_menu{make_child<fm_menu>()};
};

#endif  // TUI_YES_NO_MENU_WIDGET
