# Vapo Linux (temporary name)

A project for me to learn more about TF2 and how it works

This is made for Linux, so trying to build it on Windows or non Unix-like systems might not work!

## How to inject

Run the `attach.sh` file as **sudo**/**root**!

TF2 **should be** open and in the main menu. Currently this does not wait for the libraries to be opened so it will **silently fail** if you dont wait long enough

```bash
sudo ./attach.sh
```

## How to build

* Requirements
  * Go (at least 1.25.5)
  * Linux
  * GCC/G++ (compiled with atleast standard C++17)

### How to compile

#### GUI

Just run the `build-gui.sh` file and it should compile and open the gui

```bash
./build-gui.sh
```

#### "DLL" (Shared Library)

Run `build-so.sh` to build the .so file