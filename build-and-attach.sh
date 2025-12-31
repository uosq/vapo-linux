#!/usr/bin/env bash

g++ -shared -fPIC internal/main.cpp internal/libsigscan.c -o build/libvapo.so -O2 -s -lm

cd gui
go run . &