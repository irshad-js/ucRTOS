#!/bin/bash

export PATH="$PATH:$PWD/../lib/wxWidgets/gtk-build"

make
bin/ucrtos.elf
