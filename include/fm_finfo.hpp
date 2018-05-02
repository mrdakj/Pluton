#ifndef FM_FINFO
#define FM_FINFO

#include <cppurses/cppurses.hpp>
#include "file.hpp"

using namespace cppurses;

class Fm_finfo : public Vertical_layout {

	Label& title{this->make_child<Label>("File info")};
	Blank_height& title_blank{this->make_child<Blank_height>(1)};

	Label& file_name{this->make_child<Label>()};
	Blank_height& blank_after_name{this->make_child<Blank_height>(1)};

	Label& file_type{this->make_child<Label>()};
	Blank_height& blank_after_type{this->make_child<Blank_height>(1)};

	Label& file_size{this->make_child<Label>()};
	Blank_height& blank_after_size{this->make_child<Blank_height>(1)};

public:

	Fm_finfo(const File& f);
	Fm_finfo();

	void set_file(Glyph_string fname, Glyph_string ftype, Glyph_string fsize);
	void set_file(const File& f);

};


#endif
