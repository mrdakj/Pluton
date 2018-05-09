#ifndef TUI_TEXT_INPUT
#define TUI_TEXT_INPUT

#include <cppurses/cppurses.hpp>

using namespace cppurses;

class Fm_text_input_widget : public Vertical_layout {
private:
	class Text_input : public Line_edit {

		bool key_press_event(Key key, char symbol) override;
		bool focus_in_event() override;

	public:
		Text_input (const Glyph_string& initial_text="");
};

public:
	Label& title;
	Blank_height& title_blank{this->make_child<Blank_height>(1)};
	Label& info_message;
	//Blank_height& msg_blank{this->make_child<Blank_height>(1)};

	Text_input& text_input;

	Fm_text_input_widget(const Glyph_string &title, const Glyph_string &info_message, const Glyph_string &initial_text=""); 
};


#endif // TUI_TEXT_INPUT

