# Distributed Word Count
This repository contains code that inḿplements a distributed [MapReduce](https://en.wikipedia.org/wiki/MapReduce) algorithm to count words in a text. The implementation uses the
[ZeroMQ library](https://zeromq.org/) to communicate between the *distributor* and the *workers*. 

### How to use?

1. Clone the repository
   ```bash
   git clone https://github.com/Kolja-05/Distributed-Word-Count

2. Compile the programm
   ```bash
   cmake -B build -S .
   make -C build

3. Start the worker (each worker gets a list of the ports of all the workers)
   ```bash
   ./build/zmq_worker <port1> <port2>... <portn>
4. MStart the distributor (the distributor gets the filepath of your input as well as all the ports of the workers)
   ```bash
   ./build/zmq_distributor <filepath> <port1> <port2> ... <portn>

