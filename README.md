# GameBoy Emulator
A GameBoy Emulator written in C++ using the SDL2 library

# Install
Windows binaries are available for download in the releases section for unix compile instructions will be below

### Install packages
#### Arch-Linux
$ `sudo pacman -S sdl2 cmake`
#### Debian
$ `sudo apt-get install libsdl2-dev cmake`
#### Mac OS
$ `brew install sdl2 cmake`

### Clone and compile
$ `git clone https://github.com/KolbyML/gameboy-emulator.git`

$ `cd gameboy-emulator`

$ `cmake --configure .`

$ `cmake --build .`

Congratz 🥳🎉🎉

# Usage
Drag and drop a GameBoy rom onto gameboy-emulator.exe

or in the terminal run

``./gameboy-emulator <rom location> <optional args etc (--help, --debug)>``

# Resources
[Pandocs](https://gbdev.io/pandocs)
