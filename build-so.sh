#!/usr/bin/env bash

g++ -shared -fPIC internal/main.cpp internal/libsigscan.c internal/sdk/definitions/ienginetrace.cpp internal/libdetour/*.c -o build/libvapo.so -O2 -s -lm --std=c++17