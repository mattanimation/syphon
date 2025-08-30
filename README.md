# SYstem SiPHON

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
- `./scripts/setup.sh`

---

## BUILD

### MAC
- `make compile_lib_mac` - compile the lib
- `make create_lib_shared_mac` - turn the compiled lib into a shared lib
- `make build_example_shared_mac` - build the example using shared lib
- `make create_static_mac` - build the static lib
- `make compile_example_static_mac` - build the example static
- `make link_example_static_mac` - link the static lib with the example

### LINUX
- `make compile_lib_linux` - compile the lib
- `make create_lib_shared_linux` - turn the compiled lib into a shared lib
- `make build_shared_linux` - build the shared lib
- `make build_example_shared_linux` - build the example using shared lib
- `make create_lib_static_linux` - create the static lib
- `make build_example_static_linux` - build the example static
- `make link_example_static_linux` - link the static lib with the example

---

## RUN
- `make run`

---

## Goals

some of the goals for this project were:

- [ ] open source something to use in my own projects
- [ ] make something that is "cross platform"
    - [x] works on mac
    - [ ] works on linux 
- [x] practice systems level programming
- [x] learn how to get system info
    - [x] mac
    - [x] linux
- [x] use dynamic array in c
- [x] learn how to create a library
    - [x] use as shared library
    - [x] use as a static library


