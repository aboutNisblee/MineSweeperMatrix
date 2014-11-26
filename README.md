# MineSweeperMatrix library version 2

This project implements the game logic of the puzzle game [Minesweeper](http://en.wikipedia.org/wiki/Minesweeper_%28video_game%29).  
I've used the game as training project for years and found it very useful to solve the same problems with different programming languages and GUI libraries. 
The logic used in this library was originally used in an implementation of the game I've developed while my studies in 2011. This first version of the game used GTKmm for the GUI. Later, I would like to become acquainted with Qt. To not implement the logic again, I've extracted it from the first game.
This library is a rework of that library. I've built it at the end of 2012 for an Android implementation of Minesweeper.

In 2014 I've decided to make this project open-source. Maybe the code is useful for anybody. The implementation covers some basic topics of (C++) programming like:

* Observer pattern
* Implementation hiding with the pimpl idiom
* Advanced subscript operator overloading ([ ][ ])
* Exceptions
* boost::signals2 (optionally activatable)
* const correctness (I guess)
* boost::test

If you find some bugs, anti-pattern or anything you like to indicate, please write me an E-Mail or contribute to the project here at GitHub.

## Project structure
* /src - Source files
* /doc - Doxygen configuration file
* /example - Simple example of how to use the library
* /build - Output directory for the preconfigured builds
* /eclipse - Project files for the Eclipse

## Build system
There are two preconfigured builds. One for the example and one for unit tests.  
For this two builds CMake scripts are included to create a platform specific build script. To create the scripts switch to e.g. build/example and execute the cmake.sh script to run CMake with the correct arguments (or use the arguments from that script).

**Note:** I've only tested it under Linux. If you like to build scripts for e.g. Windows you have to at least add the compiler settings to the root CMakeLists file.

## Documentation
The project is documented inline. Use Doxygen to generate a website from that inline documentation, if you like to.  
This will produce a website in doc/html:
```shell
cd doc
doxygen Doxyfile
```
