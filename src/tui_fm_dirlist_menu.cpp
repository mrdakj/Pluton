#include "tui_fm_dirlist_menu.hpp"
#include "tui_fm_finfo.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

#include "current_dir.hpp"

Fm_dirlist_menu::Fm_dirlist_menu_item::Fm_dirlist_menu_item(Push_button& ref) : button{ref} {}

Fm_dirlist_menu::Fm_dirlist_menu(Glyph_string title)
    : title_{this->make_child<Label>(std::move(title))}
{
    this->focus_policy = Focus_policy::Strong;
    title_.set_alignment(Alignment::Center);
    title_.brush.add_attributes(Attribute::Bold);
    space1.background_tile = L'─';
}


sig::Signal<void()>& Fm_dirlist_menu::add_item(Glyph_string label)
{
    Push_button& button_ref{this->make_child<Push_button>(std::move(label))};
    button_ref.install_event_filter(this);
    items_.emplace_back(button_ref);
    button_ref.height_policy.type(Size_policy::Fixed);
    button_ref.height_policy.hint(1);
    auto& signal_ref{items_.back().selected};
    button_ref.clicked.connect(
        [this, index = items_.size() - 1] { items_[index].selected(); });

    update();
    return signal_ref;
}

sig::Signal<void()>& Fm_dirlist_menu::insert_item(Glyph_string label, std::size_t index)
{
    auto button_ptr{std::make_unique<Push_button>(std::move(label))};
    button_ptr->install_event_filter(this);
    button_ptr->height_policy.type(Size_policy::Fixed);
    button_ptr->height_policy.hint(1);
    Push_button& new_button{*button_ptr};
    items_.insert(std::begin(items_) + index, Fm_dirlist_menu_item{new_button});
    auto& signal_ref{items_[index].selected};
    new_button.clicked.connect([this, index] { items_[index].selected(); });

    update();
    return signal_ref;
}

void Fm_dirlist_menu::remove_item(std::size_t index) 
{
    if (index >= items_.size()) {
        return;
    }

    remove_child(&items_[index].button.get());
    items_.erase(std::begin(items_) + index);

	if (selected_index_ != 0)
		selected_index_--;

    update();
}

void Fm_dirlist_menu::clear()
{
	for (unsigned index = 0; index < items_.size(); index++)
		remove_child(&items_[index].button.get());
	items_.clear();
	selected_index_ = 0;
	update();
}

void Fm_dirlist_menu::select_up(std::size_t n) {

    std::size_t next_index;

    if (selected_index_ > n) {
        next_index = selected_index_ - n;
    } else {
        next_index = 0;
    }

    select_item(next_index);
}

void Fm_dirlist_menu::select_down(std::size_t n) 
{
    std::size_t next_index;	

    if (items_.empty()) {
        return;
    }
    std::size_t new_index{selected_index_ + n};
    if (new_index >= items_.size()) {
        next_index = items_.size() - 1;
    } else {
        next_index = new_index;
    }

    select_item(next_index);
}

void Fm_dirlist_menu::select_item(std::size_t index) {
    if (items_.empty()) {
        return;
    }
    if (index >= items_.size()) {
        selected_index_ = items_.size() - 1;
    } else {
        selected_index_ = index;
    }

    selected_file_changed();
    update();
}

std::size_t Fm_dirlist_menu::size() const 
{
    return items_.size();
}



Glyph_string Fm_dirlist_menu::get_selected_item_name() const
{
	Push_button & tmpbut = items_[selected_index_].button;
	return tmpbut.contents();
}


bool Fm_dirlist_menu::paint_event() {
    for (Fm_dirlist_menu_item& item : items_) {
        item.button.get().brush.remove_attribute(Attribute::Inverse);
    }
    items_[selected_index_].button.get().brush.add_attributes(
        Attribute::Inverse);
    return Vertical_layout::paint_event();
}

bool Fm_dirlist_menu::key_press_event(Key key, char symbol) {
    if (key == Key::Arrow_down || key == Key::j) {
        select_down();
    } else if (key == Key::Arrow_up || key == Key::k) {
        select_up();
    } else if (key == Key::Enter || key == Key::l) {
        call_current_item();
    } else if (key == Key::Escape || key == Key::h) {
		esc_pressed();
    } else if (key == Key::d) {
		d_pressed();
	} else if (key == Key::Space) {
	} else if (key == Key::q) {
		insert_rfile("new_file");
	} else if (key == Key::w) {
		insert_dir("new_dir");
	} else if (key == Key::r) {
		rename_selected("new_name");
	}

    return true;
}

bool Fm_dirlist_menu::mouse_press_event(Mouse_button button,
                             Point global,
                             Point local,
                             std::uint8_t device_id) {
    if (button == Mouse_button::ScrollUp) {
        select_up();
    } else if (button == Mouse_button::ScrollDown) {
        select_down();
    }
    return Widget::mouse_press_event(button, global, local, device_id);
}

bool Fm_dirlist_menu::mouse_press_event_filter(Event_handler* receiver,
                                    Mouse_button button,
                                    Point global,
                                    Point local,
                                    std::uint8_t device_id) {
    if (button == Mouse_button::ScrollUp) {
        select_up();
        return true;
    }
    if (button == Mouse_button::ScrollDown) {
        select_down();
        return true;
    }
    return false;
}

/* On press Key::Enter -> Call selected */
void Fm_dirlist_menu::call_current_item() {
    if (!items_.empty()) {
        items_[selected_index_].selected();
    }
}


sig::Slot<void(std::size_t)> select_up(Fm_dirlist_menu& m) {
    sig::Slot<void(std::size_t)> slot{[&m](auto n) { m.select_up(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> select_up(Fm_dirlist_menu& m, std::size_t n) {
    sig::Slot<void()> slot{[&m, n] { m.select_up(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> select_down(Fm_dirlist_menu& m) {
    sig::Slot<void(std::size_t)> slot{[&m](auto n) { m.select_down(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> select_down(Fm_dirlist_menu& m, std::size_t n) {
    sig::Slot<void()> slot{[&m, n] { m.select_down(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> select_item(Fm_dirlist_menu& m) {
    sig::Slot<void(std::size_t)> slot{
        [&m](auto index) { m.select_item(index); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> select_item(Fm_dirlist_menu& m, std::size_t index) {
    sig::Slot<void()> slot{[&m, index] { m.select_item(index); }};
    slot.track(m.destroyed);
    return slot;
}


