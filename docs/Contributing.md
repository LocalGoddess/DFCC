# Contributing

Anyone is welcome to contribute to DFCC!

## Grabbing The Source

In order to get the source code of this repository open a command line and enter the command
<br>`git clone --recursive https://github.com/Hammer86gn/DFCC` <br>
This will make sure all submodules are also cloned with the repository. If you forgot to clone the repository
recursively run
<br> `git submodule update --init` <br>

## Formatting

The code you make should be formatted in accordance with the [.clang-format](../.clang-format) file. Most IDEs will
follow this
or allow you to reformat the code using the [.clang-format](../.clang-format). For example, CLion has the
hotkey `ctrl + alt + l` to reformat a file.

## File Structure

The file structure for DFCC is simple; Source files go in the [src/](../src) folder and header files go in
the [inc/](../inc) folder.

*Please Note: only include source files in the CMakeLists.txt unless otherwise specified*