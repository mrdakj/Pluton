# Pluton file manager

## About project
Project made for computer science class on [Faculty of Mathematics](http://www.matf.bg.ac.rs/) (University of Belgrade).
Goal of project is to show some of the functional concepts in `c++`.

[![](screenshots/1.png?raw=true)](https://github.com/mrdakj/pluton)
[Github link](https://github.com/mrdakj/pluton)

Provided [binary](bin/pluton) run's under GNU/Linux operating system.

### Requirements
- GNU/Linux
- [Make](https://www.gnu.org/software/make/)
- [g++ with c++17 standard support](https://gcc.gnu.org/)
- [immer](https://github.com/arximboldi/immer)
- [cppurses](https://github.com/a-n-t-h-o-n-y/cppurses)

### Building binary from source code
cd in source code directory na run `make` command.

### Run program
```./pluton```

### Key bindings
| key              | function                         |
| ---------------- |:--------------------------------:|
| j, k             | Select next/previous file        |
| l, enter         | Enter selected directory         |
| h, backspace     | Go to parent directory           |
| q                | Create empty regular file        |
| w                | Create empty directory           |
| e                | Run selected file                |
| r                | Rename selected file             |
| u/?              | Undo/Redo                        |
| Esc              | Exit program                     |

### Author's:
- Jelena Mrdak (mi1521@alas.matf.bg.ac.rs)
- Nebojsa Koturovic (mi15139@alas.matf.bg.ac.rs)

### TODO List
- Selecting multiple files.
- Copying/Moving files functionality.
- Find file's functionality.
- Integrated shell (terminal) support.
- Error logging on screen.
