# Vapo Linux (temporary name)

A project for me to learn more about TF2 and how it works

This is made for Linux, so trying to build it on Windows or non Unix-like systems might not work!

## Credits

8dcc - [libdetour](https://github.com/8dcc/libdetour)

8dcc - [libsigscan](https://github.com/8dcc/libsigscan)

## How to attach

Run the `attach.sh` file as **sudo**/**root**!

TF2 **should be** open and in the main menu. Currently this does not wait for the libraries to be opened so it will **silently fail** if you dont wait long enough

```bash
sudo ./attach.sh
```

## How to build

* Requirements
  * Linux
  * GCC/G++ (compiled with atleast standard C++17)
  * Glew
  * Lua 5.4

Run `build.sh` to build the .so file

```bash
mkdir build
cp /usr/lib64/libGLEW.so.2.2 build/
cp /usr/lib64/liblua5.4.so.0 build/
./build.sh
```