#include "tui_fm_finfo.hpp"

Fm_finfo::Fm_finfo() {
   	this->title.set_alignment(Alignment::Center);
    	this->title.brush.add_attributes(Attribute::Bold);
    	this->title_blank.background_tile = L'─';
	enable_border(*this);
}

Fm_finfo::Fm_finfo(const File &f) : Fm_finfo() {
	set_file(f);
}

void Fm_finfo::set_file(Glyph_string fname, Glyph_string ftype, Glyph_string fsize) 
{
	this->file_name.set_text(Glyph_string("Name: ") + fname);
	this->file_type.set_text(Glyph_string("Type: ") + ftype);
	this->file_size.set_text(Glyph_string("Size: ") + fsize);
}


void Fm_finfo::set_file(const File& f)  
{
	auto str_file_type = [] (char c) -> Glyph_string {

		switch (c) {
			case 'r': return "regular file";
			case 'd': return "directory";
			default: 
					  return "unknown";
		}
	};

	set_file(f.get_name(), str_file_type(f.get_type()), std::to_string(f.get_size()));
}
