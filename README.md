#2048-cli

A cli version of the game [2048](https://github.com/gabrielecirulli/2048) for your Linux 
terminal.

#####2048_curses.c
![Screenshot](http://i.imgur.com/QU7t5mH.png)

#####2048_no_curses.c
![Screenshot](http://i.imgur.com/fwZEvdh.png) 

## Installation
This requires the ncurses library to link against during compilation. It is available
in most package managers. The program creates and uses a file name `.hs2048g` in the
working directory. Any file with this name will be modified and replaced.

### Get
    git clone https://github.com/Tiehuis/2048-cli.git
    make
### Run
    ./2048   

## Options
    -s <size>      Set the grid border length
    -b <rate>      Set the block spawn rate
    -r             Resets hiscore. Will prompt user
    -c             Enables color support (ncurses version only)
    -C             Disables color support (ncurses version only)

## License
This code is licensed under the [MIT License](https://github.com/Tiehuis/2048-cli/blob/master/LICENSE).
