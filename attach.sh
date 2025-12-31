#!/usr/bin/env bash

PID=$(pidof tf_linux64)
SO="/home/tevin/Documentos/Projetos/c tf2 test/build/libvapo.so"

gdb -q -n --batch -p "$PID" \
	-ex "set pagination off" \
	-ex "call (void*)dlopen(\"$SO\", 2)" \
	-ex "call (char*)dlerror()" \
	-ex "detach" \
	-ex "quit"
