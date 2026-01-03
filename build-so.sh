#!/usr/bin/env bash

#    internal/gui.cpp \

g++ -shared -fPIC \
    internal/main.cpp \
    internal/libsigscan.c \
    internal/sdk/definitions/ienginetrace.cpp \
    internal/libdetour/libdetour.c \
    -o build/libvapo.so \
    -O2 -std=c++17