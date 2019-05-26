# EchoServer
A Simple Echo Server only run on Unix/Linux.

compile:
```
g++ server_thread.cpp -o server -lpthread
gcc client.c -o client -lpthrea --std=gnu99
```

usage:
```
./server -c120
./client -v -c50 -o2 -h10000 127.0.0.1
```