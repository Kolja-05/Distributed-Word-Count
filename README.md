# Distributed Word Count

This repository contains an implementation of a distributed [**MapReduce**](https://en.wikipedia.org/wiki/MapReduce) algorithm for counting words in a text file.  
Communication between the *distributor* and the *workers* is handled using the
[ZeroMQ](https://zeromq.org/) messaging library.

---

## Overview

The system consists of:
- **Distributor**: Splits the input text, coordinates the workflow, and aggregates results.
- **Workers**: Perform the MAP and REDUCE operations on text chunks.

The implementation follows the MapReduce paradigm, with a simplified workflow that skips the SHUFFLE step.

---

## How to Use

### 1. Clone the Repository

```bash
git clone https://github.com/Kolja-05/Distributed-Word-Count
cd Distributed-Word-Count
```

### 2. Build the Project

```bash
cmake -B build -S .
make -C build
```

### 3. Start the Workers

Each worker must be started with a list of ports of **all** workers in the system:

```bash
./build/zmq_worker <port1> <port2> ... <portN>
```

You can start multiple workers in separate terminals using different ports.

### 4. Start the Distributor

The distributor requires the path to the input file and the list of worker ports:

```bash
./build/zmq_distributor <filepath> <port1> <port2> ... <portN>
```

---

## About the Algorithm

### 1. Chunking (Input Preparation)

The distributor splits the input text into chunks of **1500 bytes**.  
Each chunk is sent to a worker with a `MAP` request.


```
Distributor → "This is a first example, a great example" → Worker
```

---

### 2. MAP Phase

Each worker:
- Creates a hashmap
- Counts word occurrences
- Represents counts as strings of '1' characters


```
Worker → "this1is1a11first1example11great1" → Distributor
```

---

### 3. Collect the Results

The distributor gathers results from all workers and concatenates them.


```
Worker 1: "this1is1a11first1example11great1"
Worker 2: "another1great1example"
```

Concatenated result:

```
"this1is1a11first1example11great1another1great1example"
```

---

### 4. REDUCE Request

The concatenated string is sent to a worker with a `REDUCE` request.

---

### 5. REDUCE Phase

The worker:
- Converts strings of '1' characters into numeric counts
- Aggregates counts if words are identical words
- Sends the reduced result back to the distributor

---

### 6. COMBINE Phase (Final Aggregation)

Because the SHUFFLE phase is skipped, the distributor has to aggregate once more.


```
this,1
is,1
a,2
first,1
example,3
great,2
another,1
```

---

## Notes

- Skipping SHUFFLE phase simplifies the architecture but requires extra aggregation during the combine step.
- The concatenated results of the `MAP`-phase are much smaller than the original input chunks, reducing network overhead.

