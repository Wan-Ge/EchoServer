#!/bin/sh
g++ server_thread.cpp -o server -lpthread
gcc client.c -o client -lpthread --std=gnu99
