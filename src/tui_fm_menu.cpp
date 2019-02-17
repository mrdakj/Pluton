#include "tui_fm_menu.hpp"
#include <algorithm>
#include <fstream>
#define unused(x) ((void)x)

fm_menu::fm_menu_item::fm_menu_item(Push_button& ref) : button{ref} {}

fm_menu::fm_menu()
	: selected_index_(0)
{
	focus_policy = Focus_policy::Strong;
}

void fm_menu::set_items(const std::vector<std::tuple<file, opt::Optional<sig::Slot<void()>>>> &items) 
{
	clear();
	for_each(items.cbegin(), items.cend(), [this](auto item) {
			auto & sig = add_item(std::get<0>(item).name(), std::get<0>(item).is_dir());

			auto& opt_slot = std::get<1>(item);
			if (opt_slot != opt::none)
				sig.connect(opt_slot.value());
	});
}

sig::Signal<void()>& fm_menu::add_item(Glyph_string label)
{
	Push_button& button_ref{make_child<Push_button>(std::move(label))};
	button_ref.install_event_filter(this);
	items_.emplace_back(button_ref);
	button_ref.height_policy.type(Size_policy::Fixed);
	button_ref.height_policy.hint(1);
	auto& signal_ref{items_.back().selected};
	button_ref.clicked.connect([this, index = items_.size() - 1] { items_[index].selected(); });

	update();
	return signal_ref;
}

sig::Signal<void()>& fm_menu::add_item(Glyph_string label, bool is_dir)
{
	Push_button& button_ref{make_child<Push_button>(std::move(label))};
	if (is_dir)
		button_ref.brush.set_foreground(Color::Blue);
	else
		button_ref.brush.set_foreground(Color::Green);

	button_ref.install_event_filter(this);
	items_.emplace_back(button_ref);
	button_ref.height_policy.type(Size_policy::Fixed);
	button_ref.height_policy.hint(1);
	auto& signal_ref{items_.back().selected};
	button_ref.clicked.connect([this, index = items_.size() - 1] { items_[index].selected(); });

	update();
	return signal_ref;
}

sig::Signal<void()>& fm_menu::insert_item(Glyph_string label, std::size_t index)
{
	auto button_ptr{std::make_unique<Push_button>(std::move(label))};
	button_ptr->install_event_filter(this);
	button_ptr->height_policy.type(Size_policy::Fixed);
	button_ptr->height_policy.hint(1);
	Push_button& new_button{*button_ptr};
	items_.insert(std::begin(items_) + index, fm_menu_item{new_button});
	auto& signal_ref{items_[index].selected};
	new_button.clicked.connect([this, index] { items_[index].selected(); });

	update();
	return signal_ref;
}

void fm_menu::remove_item(std::size_t index) 
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

void fm_menu::clear()
{
	for (unsigned index = 0; index < items_.size(); index++)
		remove_child(&items_[index].button.get());
	items_.clear();
	selected_index_ = 0;
	update();
}

void fm_menu::select_up(std::size_t n)
{
	std::size_t next_index;

	if (selected_index_ > n) {
		next_index = selected_index_ - n;
	}
	else {
		next_index = 0;
	}

	select_item(next_index);
}


void fm_menu::select_down(std::size_t n) 
{
	std::size_t next_index;	

	if (items_.empty()) {
		return;
	}
	std::size_t new_index{selected_index_ + n};
	if (new_index >= items_.size()) {
		next_index = items_.size() - 1;
	}
	else {
		next_index = new_index;
	}

	select_item(next_index);
}

void fm_menu::select_item(std::size_t index)
{
	if (items_.empty()) {
		return;
	}

	// set_foreground(items_[selected_index_].button.get(), Color::White);
	selected_index_ = (index >= items_.size()) ? items_.size() - 1 : index;
	// set_foreground(items_[selected_index_].button.get(), Color::Yellow);

	selected_item_changed();
	update();
}

std::size_t fm_menu::size() const 
{
	return items_.size();
}


std::size_t fm_menu::selected_index() const
{
	return selected_index_;
}

void fm_menu::change_selected(std::size_t index)
{
	selected_index_ = index;
}

Glyph_string fm_menu::selected_item_name() const
{
	Push_button & tmpbut = items_[selected_index_].button;
	return tmpbut.contents();
}


bool fm_menu::paint_event()
{
	if (items_.empty())
		return Vertical_layout::paint_event();

	// remove previous styles (colors...)
	for (fm_menu_item& item : items_) {
		item.button.get().brush.remove_attribute(Attribute::Inverse);
	}

	// set new styles for selected button
	items_[selected_index_].button.get().brush.add_attributes(Attribute::Inverse);

	return Vertical_layout::paint_event();
}


bool fm_menu::resize_event(Area new_size, Area old_size)
{
	update();
	return Vertical_layout::resize_event(new_size, old_size);
}


bool fm_menu::key_press_event(Key key, char symbol)
{
	unused(symbol);
	if (key == Key::Arrow_down || key == Key::j) {
		select_down();
	}
	else if (key == Key::Arrow_up || key == Key::k) {
		select_up();
	}
	else if (key == Key::Enter || key == Key::l) {
		call_current_item();
	}
	else if (key == Key::Escape) {
		esc_pressed();
	}

	return true;
}

bool fm_menu::mouse_press_event(Mouse_button button,
		Point global,
		Point local,
		std::uint8_t device_id)
{
	if (button == Mouse_button::ScrollUp) {
		select_up();
	}
	else if (button == Mouse_button::ScrollDown) {
		select_down();
	}

	return Widget::mouse_press_event(button, global, local, device_id);
}

bool fm_menu::mouse_press_event_filter(Event_handler* receiver,
		Mouse_button button,
		Point global,
		Point local,
		std::uint8_t device_id)
{
	unused(receiver);
	unused(global);
	unused(local);
	unused(device_id);

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
void fm_menu::call_current_item() const
{
	if (!items_.empty()) {
		items_[selected_index_].selected();
	}
}


std::size_t fm_menu::menu_height() const
{
	return height();
}


sig::Slot<void(std::size_t)> select_up(fm_menu& m)
{
	sig::Slot<void(std::size_t)> slot{[&m](auto n) { m.select_up(n); }};
	slot.track(m.destroyed);
	return slot;
}

sig::Slot<void()> select_up(fm_menu& m, std::size_t n)
{
	sig::Slot<void()> slot{[&m, n] { m.select_up(n); }};
	slot.track(m.destroyed);
	return slot;
}

sig::Slot<void(std::size_t)> select_down(fm_menu& m)
{
	sig::Slot<void(std::size_t)> slot{[&m](auto n) { m.select_down(n); }};
	slot.track(m.destroyed);
	return slot;
}

sig::Slot<void()> select_down(fm_menu& m, std::size_t n)
{
	sig::Slot<void()> slot{[&m, n] { m.select_down(n); }};
	slot.track(m.destroyed);
	return slot;
}

sig::Slot<void(std::size_t)> select_item(fm_menu& m)
{
	sig::Slot<void(std::size_t)> slot{ [&m](auto index) { m.select_item(index); }};
	slot.track(m.destroyed);
	return slot;
}

sig::Slot<void()> select_item(fm_menu& m, std::size_t index)
{
	sig::Slot<void()> slot{[&m, index] { m.select_item(index); }};
	slot.track(m.destroyed);
	return slot;
}
