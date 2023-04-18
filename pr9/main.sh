#!/bin/bash
g++ -c main.cpp
g++ -o main main.o -lcap

touch file
sudo chown root file
sudo chgrp root file