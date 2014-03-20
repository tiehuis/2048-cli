#2048-cli

A cli version of the game [2048](https://github.com/gabrielecirulli/2048) for your Linux terminal.

#####2048_curses.c
![Screenshot](http://i.imgur.com/QU7t5mH.png)

#####2048_no_curses.c
![Screenshot](http://i.imgur.com/fwZEvdh.png) 

## Installation
If you want to use the ncurses version, make sure that you have the required ncurses library and link against this during compilation with `-lcurses`. The program creates and uses a file named `.hs2048g` in the directory it is run. Ensure that you have no file of this name in the directory otherwise it will be overwritten.
### Get
    cd ~/$INSTALL_DIR
    wget https://raw.githubusercontent.com/Tiehuis/2048-cli/master/2048_no_curses.c
    gcc 2048_no_curses.c -o 2048
### Run
    ./2048   

## Options
    -s <size>      Set the grid border length
    -b <rate>      Set the block spawn rate
    -r             Resets hiscore. Will prompt user
    -c             Enables color support (ncurses version only)

## License
This code is licensed under the [MIT License](https://github.com/Tiehuis/2048-cli/blob/master/LICENSE).
