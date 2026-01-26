#!/usr/bin/env bash

if [ ! -d build ]; then
	mkdir build
fi

# check for Pluto-lang static binary
# if not there, compile it
if [ ! -f build/libplutostatic.a ]; then
	cd build

	wget https://github.com/PlutoLang/Pluto/archive/refs/tags/0.12.2.zip
	unzip "0.12.2.zip"
	rm "0.12.2.zip"

	cd Pluto-0.12.2

	make -j PLAT=linux

	cp src/libplutostatic.a ../

	cd ../
	rm -fr Pluto-0.12.2
	cd ../
fi

# check for Lua static binary
# if not there, compile it
#if [ ! -f build/liblua.a ]; then
	#cd build

	#if [ ! -d lua-5.4.8 ]; then
		#wget https://www.lua.org/ftp/lua-5.4.8.tar.gz
		#tar -xzf lua-5.4.8.tar.gz
		#rm lua-5.4.8.tar.gz
	#fi

	#cd lua-5.4.8

	#make -j$(nproc) linux MYCFLAGS="-fPIC" MYLDFLAGS="-fPIC"
	#cp src/liblua.a "../"

	#cd ..

	#rm -fr lua-5.4.8
#fi

# check for glew (opengl)
# if not there, compile it
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

# check for netvar text file (I should probably generate it on the fly from the already made SetupNetvars function)
if [ ! -f build/netvars.txt ]; then
	cp netvars.txt build/
fi

# copy our p100 attach script
cp attach.sh build/

# copy our p100 docs
cp -r lua-docs build/

chmod +x build/attach.sh

# compile it
# this shit takes longer to compile than I want to admit
# i shouldn't include the entire pluto and glew libraries
# but it doesn't attach without them
# fuck my life
g++ -shared -fPIC \
	-Wl,--whole-archive \
	build/libGLEW.a \
	build/libplutostatic.a \
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
	src/features/aimbot/aimbot.cpp \
	src/settings.cpp \
	-o build/libvapo.so \
	-O2 -std=c++17 -lSDL2 -lvulkan -lm -ldl \
	-Werror -flto=auto -g