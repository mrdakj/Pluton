#ifndef TUI_FM_TEXT_INPUT
#define TUI_FM_TEXT_INPUT

#include <cppurses/cppurses.hpp>

using namespace cppurses;

class fm_text_input_widget : public Vertical_layout {
	private:
		class Text_input : public Line_edit {

			bool key_press_event(Key key, char symbol) override;
			bool focus_in_event() override;
			bool focus_out_event() override;

			public:
			Text_input (const Glyph_string& initial_text="");
			sig::Signal<void()> editing_canceled;

		};

	public:
		Label& title;
		Blank_height& title_blank{this->make_child<Blank_height>(1)};
		Label& info_message;
		//Blank_height& msg_blank{this->make_child<Blank_height>(1)};

		Text_input& text_input;

		sig::Signal<void(std::string)> &editing_finished;
		sig::Signal<void()> &editing_canceled;
		fm_text_input_widget(const Glyph_string &title, const Glyph_string &info_message, const Glyph_string &initial_text=""); 

		void grab_focus(); 
		void change_title(const std::string& new_title);
		void change_info_message(const std::string& message);
};

#endif // TUI_FM_TEXT_INPUT

