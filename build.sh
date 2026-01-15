#!/usr/bin/env bash

if [ ! -d build ]; then
	mkdir build
fi

if [ ! -f build/liblua.a ]; then
	cd build

	if [ ! -d lua-5.4.8 ]; then
		wget https://www.lua.org/ftp/lua-5.4.8.tar.gz
		tar -xzf lua-5.4.8.tar.gz
		rm lua-5.4.8.tar.gz
	fi

	cd lua-5.4.8

	make -j$(nproc) linux MYCFLAGS="-fPIC" MYLDFLAGS="-fPIC"
	cp src/liblua.a "../"

	cd ..

	rm -fr lua-5.4.8
fi

if [ ! -f build/libGLEW.a ]; then
	cd build

	if [ ! -d glew-2.3.0 ]; then
		wget https://github.com/nigels-com/glew/releases/download/glew-2.3.0/glew-2.3.0.zip
		unzip glew-2.3.0.zip
		rm glew-2.3.0.zip
	fi

	cd glew-2.3.0
	cd build

	cmake ./cmake \
  		-DBUILD_SHARED_LIBS=OFF \
  		-DCMAKE_POSITION_INDEPENDENT_CODE=ON

	make -j$(nproc) glew_s
	cp lib/libGLEW.a ../../

	cd ../../../

	rm build/glew-2.3.0
fi

if [ ! -f build/netvars.txt ]; then
	cp netvars.txt build/
fi

g++ -shared -fPIC \
	-Wl,--whole-archive \
	build/libGLEW.a \
	build/liblua.a \
	-Wl,--no-whole-archive \
	src/main.cpp \
	src/libsigscan.c \
	src/libdetour/libdetour.c \
	src/imgui/*.cpp \
	src/sdk/definitions/*.cpp \
	src/features/lua/*.cpp \
	src/sdk/interfaces/*.cpp \
	src/gui/*.cpp \
	src/features/entitylist/*.cpp \
	-o build/libvapo.so \
	-O2 -std=c++17 -lSDL2 -lvulkan -lm -ldl \
	-Werror -flto=auto