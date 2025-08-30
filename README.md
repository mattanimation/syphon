# SYstem siPHON

Syphon
A simple system info gatherer

**NOTE: this is very much a WIP**

things to gather...
- cpu
    - cores
    - percent usage
- processes
    - running
- memory percent usage, free
- network bytes tx, rx
- diskspace / swap
    - SD?
- connected devices
    - spi?
    - tty / UART?
    - CAN?
    - SD?

This is an example that creates a library and then has a file link against that library and use it.

using the `-I./lib/include` was the trick so that `#include <syphon.h>` could be used instead of using "
this means it's not looking for the relative location, becuase it tells the compiler wher the public header lives

## DEPENDENCIES

- `Make`

### Mac
- `clang`

### Linux
- `gcc`

---

## SETUP
Run this before running the other make commands
- `./scripts/setup.sh` - creates the build and bin dirs

---

## BUILD

make commands
- `make all` - all the things
- `make lib_shared` - turn the compiled lib into a shared lib
- `make example_shared` - build the example using shared lib
- `make lib_static` - build the static lib
- `make example_static` - build the example

---

## RUN
- `make run_shared` - run the example that uses the shared lib
- `make run_static` - run the example that uses the static lib

## CLEAN
- `make clean` - clean up
- `make clean_all` - clean all
---

## Goals

some of the goals for this project were:

- [ ] open source something to use in my own projects
- [ ] make something that is "cross platform"
    - [x] works on mac
    - [x] works on linux 
- [x] practice systems level programming
- [x] learn how to get system info
    - [x] mac
    - [x] linux
- [x] use dynamic array in c
- [x] learn how to create a library
    - [x] use as shared library
    - [x] use as a static library


