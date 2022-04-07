# CppND-System-Monitor

This repository contains a re-implementation of the Linux htop system monitor (as stripped down version) in C++. This project was part of the Udacity Nano Degree program for C++ and was the entry project with focus to familiarize onself with some basic concepts of C++. A starter code for this project was given.

Building and running the executable will only work under Linux, since the system monitor parses multiple files of the Linux file system to extract the process information, which will be displayed. The window to show the system's processes will then look as follows:

![System Monitor](images/monitor.png)

## Dependencies for Running Locally
* cmake >= 2.6
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac)
  * Linux: make is installed by default on most Linux distros
* gcc/g++ >= 7.4.0
  * Linux: gcc / g++ is installed by default on most Linux distros
* ncurses
  * [ncurses](https://www.gnu.org/software/ncurses/) is a library that facilitates text-based graphical output in the terminal. This project relies on ncurses for display output.
  * Install ncurses within your own Linux environment: `sudo apt install libncurses5-dev libncursesw5-dev`

## Compiling and Running

### Compiling
To compile the project, first, create a `build` directory and change to that directory:
```
mkdir build && cd build
```
From within the `build` directory, then run `cmake` and `make` as follows:
```
cmake ..
make
```
### Running
The executable will be placed in the `build` directory and can be run under a Linux system as follows:
```
./build/monitor
```