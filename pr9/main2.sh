#!/bin/bash
g++ -c main2.cpp
g++ -o main2 main2.o -lcap

sudo setcap CAP_SETFCAP=+eip main2

