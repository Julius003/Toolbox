# GraphTest

---

Evalute a graph algorithm by comparing it against another algorithm 
on randomly generated (undirected as of now) Graphs.
The speed and output will be compared.

### Usecase
I use this tool for optimizing algorithms on graphs that I have to 
do for my univerity class 'Algorithm Engineering'.

These Algorithms read the graph through the standard input and 
give the result through the standard output.

The input Graph is passed to the process in this form:
first line are two integers n and m. 
n is the number of nodes in the graph (1, ... , n).
m is the number of edges in the graph

Then m lines follow with 3 integers each.
The first integer is the source node, then the destination node, then the edge weight.

Then there follow 2 integer (aka two distinct nodes)
in case the algorithm needs them (for example source and sink in a flow network).

### Installation
Install progadd fist
```bash
g++ ./graphtest.cc -o graphtest -std=c++17
progadd ./graphtest
```

### Usage
```bash
graphtest ./myTestAlgorithm ./myAlreadyWorkingAlgorithm --min 10 --max 20 -t 5 --verbose
```

This executes ./myTestAlgorithm and compares it's output to ./myAlreadyWorkingAlgorithm.
These are tested on five graphs (the -t 5) with 10 (--min 10) nodes. And then they are tested
on five graphs with 20 (--max 20) nodes. 
The first time, the graphs will be sparse. The second time they will be dense.

*--verbose* outputs details on the individual test like **Time, Output, Status**
*--show_input* show the input the graphs were running on