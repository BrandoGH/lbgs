#!/bin/bash

if  [ -e "./Makefile" ];then
    make $*
elif [ -e "./build.ninja" ];then
    ninja -v $*
else
    echo Current generator is not support!!
fi