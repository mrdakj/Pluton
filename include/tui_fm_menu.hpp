#ifndef TUI_FM_MENU
#define TUI_FM_MENU

#include <vector>
#include <signals/slot.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/blank_height.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

using namespace cppurses;

class fm_menu : public Vertical_layout {
	private:
		std::size_t selected_index_;

		struct fm_menu_item {
			explicit fm_menu_item(Push_button& ref);
			std::reference_wrapper<Push_button> button;

			// press enter signal
			sig::Signal<void()> selected;
		};

		std::vector<fm_menu_item> items_;

	public:
		sig::Signal<void()> selected_item_changed;
		sig::Signal<void()> items_begin_boundary;
		sig::Signal<void()> items_end_boundary;
		sig::Signal<void()> esc_pressed;

		sig::Signal<void()>& add_item(Glyph_string label);
		sig::Signal<void()>& insert_item(Glyph_string label, std::size_t index);

		fm_menu();

		// get selected_index_
		std::size_t selected_index() const;
		// set selected_index_
		void change_selected(std::size_t index);
		void select_item(std::size_t index);
		void remove_item(std::size_t index);
		void clear();
		void select_up(std::size_t n = 1);
		void select_down(std::size_t n = 1);
		void set_items(const std::vector< std::tuple<const Glyph_string, opt::Optional<sig::Slot<void()>>>> &items);
		void call_current_item() const;
		virtual std::size_t menu_height() const;
		std::size_t size() const;
		Glyph_string selected_item_name() const;

	protected:
		bool paint_event() override;
		bool resize_event(Area new_size, Area old_size) override;

		bool key_press_event(Key key, char symbol) override;

		bool mouse_press_event(Mouse_button button,
				Point global,
				Point local,
				std::uint8_t device_id) override;

		bool mouse_press_event_filter(Event_handler* receiver,
				Mouse_button button,
				Point global,
				Point local,
				std::uint8_t device_id) override;
};


sig::Slot<void(std::size_t)> select_up(fm_menu& m);
sig::Slot<void()> select_up(fm_menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_down(fm_menu& m);
sig::Slot<void()> select_down(fm_menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_item(fm_menu& m);
sig::Slot<void()> select_item(fm_menu& m, std::size_t index);

#endif // TUI_FM_MENU
