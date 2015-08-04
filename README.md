#2048-cli

A cli version/engine of the game [2048](https://github.com/gabrielecirulli/2048) for your Linux
terminal.

![Screenshot](http://i.imgur.com/qTfXP4J.png)

There currently are 3 versions that can be run. These include a straight-forward terminal
based, and two using the ncurses and SDL libraries. To add a new graphical version, simply
create a .c file which implements all the functions in gfx.h and add a Makefile entry.

### Get
    git clone https://github.com/Tiehuis/2048-cli.git
    make

By default, termios.h is expected to be present for the standard terminal version, and
a VT100 compatible terminal is being used. If a VT100 terminal is not available, simply
alter the makefile and remove the '-DVT100' flag. Compiling with ncurses requires
the development libraries for it, and likewise for using SDL2.

On an ubuntu/debian machine for example, these can be obtained via

    apt-get install libncurses5-dev
    apt-get install libsdl2-dev libsdl2-ttf-dev

Note: The sdl version is largely to demonstrate plugging in an alternative
graphics frontend, and is likely not wanted. The ncurses version is recommended.

You can also also easily install this on el >= 5 (CentOS, RedHat Enterprise Linux,
Scientific Linux, Oracle) and Fedora >= 19 using the package-manager:

    sudo yum install 2048-cli[-nocurses]

For el you will need to have the
[EPEL-repository](https://fedoraproject.org/wiki/EPEL/FAQ#How_can_I_install_the_packages_from_the_EPEL_software_repository.3F)
enabled.

## Controls
    hjkl and wasd  Default movement keys
    q              Quit the current game

## Options
    -h             Print the program usage.
    -H             Print the current highscore.
    -C             Disable color support (default).
    -c             Enable color support if supported.
    -a             Enable animations (default).
    -A             Disable animations.
    -i             Enable ai without displaying game.
    -I             Enable ai and display game.
    -r             Reset highscore. Will prompt user.
    -s SIZE        Set the size of the playing field.
    -b RATE        Set the rate at which blocks spawn per turn.

Fonts used in SDL version can be found [here](http://www.openfontlibrary.org).

## License
This code is licensed under the
[MIT License](https://github.com/Tiehuis/2048-cli/blob/master/LICENSE).
