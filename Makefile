pluton: main.cpp current_dir.hpp file.hpp
	g++ -std=c++17 main.cpp -lstdc++fs -o $@

.PHONY: clean

clean:
	rm -rf pluton *.gch
