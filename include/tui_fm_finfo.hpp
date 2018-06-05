#ifndef TUI_FM_FINFO
#define TUI_FM_FINFO

#include <cppurses/cppurses.hpp>
#include "file.hpp"
#include "current_dir.hpp"

using namespace cppurses;

class fm_finfo : public Vertical_layout {


	Label& title{make_child<Label>("Info")};
	Blank_height& title_blank{make_child<Blank_height>(1)};

	Label& file_name{make_child<Label>()};
	Blank_height& blank_after_name{make_child<Blank_height>(1)};

	Label& file_type{make_child<Label>()};
	Blank_height& blank_after_type{make_child<Blank_height>(1)};

	Label& file_size{make_child<Label>()};
	Blank_height& blank_after_size{make_child<Blank_height>(1)};


public:

	fm_finfo(const file& f);
	fm_finfo();

	void set_file(Glyph_string fname, Glyph_string ftype, Glyph_string fsize);
	void set_file(const file& f);

};


#endif // TUI_FM_FINFO
