# TODO list:
2) Funkcije:  binary_find,  binary_search,  binary_lower  koje se nalaze u include/current_dir.hpp prebaciti u neki utility ili ih staviti u klasu i napraviti metode od njih --- *Trenutno se nalaze u current_dir.cpp*
4) Videti da li je bolje u metodama ~~cd~~, rename, inset_file, delete_file, *_on_system koristiti File ili string za argument --- *Za cd ubacena funkcija koja prima fs::path*
5) Implementirati metode && gde pise implement this
6) popraviti metodu insert_file -> trenutno moze da insertuje fajl sa istim imenom kao dir iako to nije moguce na sistemu
7) Razmisliti o promeni klase File (mozda neko nasledjivanje?)
8) obrada gresaka
9) Precistiti dobro klasu Current_dir i videti kako da se poveze sa TUI-jem, i jos razmisliti o citavom dizajnu programa.
10) Smisliti funkcionalnost i U/I citave aplikacije, kako ce i sta da radi, biti sto kreativniji. :)
11) TUI --- Tui treba da pretrpi ogromne izmene, mozda ce biti i u potpunosti odradjen iz pocetka... 
