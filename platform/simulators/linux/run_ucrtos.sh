#!/bin/bash

export WXDIR=$PWD/../lib/wxWidgets/gtk-build

if [ -d $WXDIR ]
then
    echo "Directory $WXDIR exists."
else
    echo "Error: Directory $WXDIR does not exists."

    mkdir ../lib/wxWidgets/gtk-build
    pushd ../lib/wxWidgets/gtk-build
    ../configure --prefix=$PWD/../lib/gtk3_so --with-gtk=3 --with-opengl
    make -j4
    make install
    popd
fi

export PATH="$PATH:$PWD/../lib/wxWidgets/gtk-build"

if make; then
    bin/ucrtos.elf
else
    echo build failed.
fi

