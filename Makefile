complie_lib_mac:
	clang -fPIC -c -I./lib/include ./lib/src/syphon.c -o ./build/syphon.o

complie_lib_linux:
	gcc -fPIC -c -I./lib/include ./lib/src/syphon.c -o ./build/syphon.o

create_lib_shared_mac:
	clang -dynamiclib -install_name @rpath/libsyphon.dylib -o ./build/libsyphon.dylib ./build/syphon.o

create_lib_shared_linux:
	gcc -shared -o ./build/libsyphon.so ./build/syphon.o

build_example_shared_mac:
	clang ./example/src/main.c -L./lib -lsyphon -Wl,-rpath,./ -o ./example/bin/syphon_shared

build_example_shared_linux:
	gcc ./example/src/main.c -L./lib -lsyphon -o ./example/bin/syphon_shared


create_lib_static_mac:
	ar rcs ./build/libsyphon.a ./build/syphon.o 

create_lib_static_linux:
	gcc -c -I./lib/include ./lib/src/syphon.c -o ./build/syphon.o


compile_example_static_mac:
	clang -c -I./lib/include ./example/src/main.c -o ./build/main.o

compile_example_static_linux:
	gcc -c -I./lib/include ./example/src/main.c -o ./build/main.o

link_example_static_mac:
	clang ./build/main.o ./build/libsyphon.a -o ./example/bin/syphon

link_example_static_linux:
	gcc ./build/main.o ./build/libsyphon.a -o ./example/bin/syphon


run_example:
	./example/bin/syphon

run_example_static:
	./example/bin/syphon_static

clean_binaries:
	rm -rf ./build/* && rm -rf ./example/bin/*

clean_libraries:
	rm -rf ./build/*
	