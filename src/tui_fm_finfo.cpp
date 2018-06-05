#include "tui_fm_finfo.hpp"

fm_finfo::fm_finfo()
{
	title.set_alignment(Alignment::Center);
	title.brush.add_attributes(Attribute::Bold);
	title_blank.background_tile = L'─';
	enable_border(*this);
}

fm_finfo::fm_finfo(const file &f) : fm_finfo() {
	set_file(f);
}

void fm_finfo::set_file(Glyph_string fname, Glyph_string ftype, Glyph_string fsize) 
{
	file_name.set_text(Glyph_string("Name: ") + fname);
	file_type.set_text(Glyph_string("Type: ") + ftype);
	file_size.set_text(Glyph_string("Size: ") + fsize);
}


void fm_finfo::set_file(const file& f)  
{
	auto str_file_type = [] (char c) -> Glyph_string {

		switch (c) {
			case 'r': return "regular file";
			case 'd': return "directory";
			default: 
					  return "unknown";
		}
	};

	set_file(f.name(), str_file_type(f.type()), std::to_string(f.size()));
}
