# ------------------------------------------------------------
# Variables – change only these if you move directories
# ------------------------------------------------------------
CC      := gcc               # default compiler (Linux)
CXX     := clang             # default compiler (macOS)
AR      := ar
CFLAGS  := -Wall -Werror -fPIC
INCLUDE := -I./lib/include
BUILD   := ./build
EXAMPLE := ./example
LIBNAME := syphon

# Platform detection (override with `make PLATFORM=mac` if needed)
UNAME   := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC      := clang
    CXX     := clang
    SHARED_FLAGS := -dynamiclib -install_name @rpath/lib$(LIBNAME).dylib
    RPATH_FLAG   := -Wl,-rpath,@loader_path/../build
else
    SHARED_FLAGS := -shared -Wl,-soname,lib$(LIBNAME).so
    RPATH_FLAG   :=
endif

# ------------------------------------------------------------
# Phony targets
# ------------------------------------------------------------
.PHONY: all \
        lib_shared lib_static \
        example_shared example_static \
        run_shared run_static \
        clean clean_all

# Default target builds everything
all: lib_shared lib_static example_shared example_static

# ------------------------------------------------------------
# Build objects (single source -> single object)
# ------------------------------------------------------------
$(BUILD)/syphon.o: ./lib/src/syphon.c ./lib/include/*.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD)/main.o: ./example/src/main.c ./lib/include/*.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# ------------------------------------------------------------
# Static library
# ------------------------------------------------------------
lib_static: $(BUILD)/lib$(LIBNAME).a

$(BUILD)/lib$(LIBNAME).a: $(BUILD)/syphon.o
	$(AR) rcs $@ $^

# ------------------------------------------------------------
# Shared library
# ------------------------------------------------------------
lib_shared: $(BUILD)/lib$(LIBNAME).so   # Linux name; on macOS this expands to .dylib

# The pattern below works for both platforms because SHARED_FLAGS is set accordingly
$(BUILD)/lib$(LIBNAME).so: $(BUILD)/syphon.o
	$(CC) $(SHARED_FLAGS) -o $@ $^

# ------------------------------------------------------------
# Example programs
# ------------------------------------------------------------
example_shared: $(EXAMPLE)/bin/$(LIBNAME)_shared

$(EXAMPLE)/bin/$(LIBNAME)_shared: ./example/src/main.c $(BUILD)/lib$(LIBNAME).so
	$(CC) $(CFLAGS) $(INCLUDE) $< -L$(BUILD) -l$(LIBNAME) $(RPATH_FLAG) -o $@

example_static: $(EXAMPLE)/bin/$(LIBNAME)_static

$(EXAMPLE)/bin/$(LIBNAME)_static: $(BUILD)/main.o $(BUILD)/lib$(LIBNAME).a
	$(CC) $(CFLAGS) $(INCLUDE) $(BUILD)/main.o $(BUILD)/lib$(LIBNAME).a -o $@

# ------------------------------------------------------------
# Run helpers (useful for quick testing)
# ------------------------------------------------------------
run_shared: example_shared
	$(EXAMPLE)/bin/$(LIBNAME)_shared

run_static: example_static
	$(EXAMPLE)/bin/$(LIBNAME)_static

# ------------------------------------------------------------
# Cleaning
# ------------------------------------------------------------
clean:
	rm -rf $(BUILD)/* $(EXAMPLE)/bin/*

clean_all: clean
	@echo "All generated files removed."


#----- old for reference -----

# compile_lib_mac:
# 	clang -fPIC -c -I./lib/include ./lib/src/syphon.c -o ./build/syphon.o

# compile_lib_linux:
# 	gcc -Wall -Werror -fPIC -c -I./lib/include ./lib/src/syphon.c -o ./build/syphon.o

# create_lib_shared_mac:
# 	clang -dynamiclib -install_name @rpath/libsyphon.dylib -o ./build/libsyphon.dylib ./build/syphon.o

# create_lib_shared_linux:
# 	gcc -shared -o -Wl,-soname,libsyphon.so ./build/syphon.o

# build_example_shared_mac:
# 	clang ./example/src/main.c -L./build -lsyphon -Wl,-rpath,./ -o ./example/bin/syphon_shared

# build_example_shared_linux:
# 	gcc ./example/src/main.c -L./build -lsyphon -o ./example/bin/syphon_shared


# create_lib_static_mac:
# 	ar rcs ./build/libsyphon.a ./build/syphon.o 

# create_lib_static_linux:
# 	gcc -c -I./lib/include ./lib/src/syphon.c -o ./build/syphon.o


# link_example_static_mac:
# 	clang ./build/main.o ./build/libsyphon.a -o ./example/bin/syphon

# link_example_static_linux:
# 	gcc ./build/main.o ./build/syphon.o -o ./example/bin/syphon


# run_example:
# 	./example/bin/syphon

# run_example_static:
# 	./example/bin/syphon_static

# clean_binaries:
# 	rm -rf ./build/* && rm -rf ./example/bin/*

# clean_libraries:
# 	rm -rf ./build/*
	
