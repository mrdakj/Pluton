#ifndef TUI_FM_DIRLIST_MENU
#define TUI_FM_DIRLIST_MENU

#include "tui_fm_menu.hpp"

using namespace cppurses;

class Fm_dirlist_menu : public Fm_menu {
   public:
    Fm_dirlist_menu(Glyph_string title);

    sig::Signal<void()>& selected_file_changed{Fm_menu::selected_item_changed};
    sig::Signal<void()>& esc_pressed{Fm_menu::esc_pressed};
    sig::Signal<void()> d_pressed;
    sig::Signal<void()> backspace_pressed;
    sig::Signal<void()> h_pressed;
    sig::Signal<void()> insert_rfile;
    sig::Signal<void()> insert_dir;
    sig::Signal<void()> rename_selected;
    sig::Signal<void()> run_file;

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
};

#endif  // TUI_FM_MENU
