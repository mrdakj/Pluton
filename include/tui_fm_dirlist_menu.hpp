#ifndef TUI_FM_DIRLIST_MENU
#define TUI_FM_DIRLIST_MENU
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/blank_height.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

using namespace cppurses;

class Fm_dirlist_menu : public Vertical_layout {
   public:
    explicit Fm_dirlist_menu(Glyph_string title);

    sig::Signal<void()>& add_item(Glyph_string label);
    sig::Signal<void()>& insert_item(Glyph_string label, std::size_t index);
    sig::Signal<void()> selected_file_changed;

    void remove_item(std::size_t index);

    void select_up(std::size_t n = 1);
    void select_down(std::size_t n = 1);
    void select_item(std::size_t index);

    std::size_t size() const;
   Glyph_string get_selected_item_name() const;

   protected:
    bool paint_event() override;

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


   private:
    struct Fm_dirlist_menu_item {
        explicit Fm_dirlist_menu_item(Push_button& ref);
        std::reference_wrapper<Push_button> button;

	/* Press enter signal */
        sig::Signal<void()> selected;
    };

    std::vector<Fm_dirlist_menu_item> items_;
    std::size_t selected_index_{0};


    Label& title_;
    Blank_height& space1{this->make_child<Blank_height>(1)};

    void call_current_item();
};


sig::Slot<void(std::size_t)> select_up(Fm_dirlist_menu& m);
sig::Slot<void()> select_up(Fm_dirlist_menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_down(Fm_dirlist_menu& m);
sig::Slot<void()> select_down(Fm_dirlist_menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_item(Fm_dirlist_menu& m);
sig::Slot<void()> select_item(Fm_dirlist_menu& m, std::size_t index);


#endif  // TUI_FM_DIRLIST_MENU