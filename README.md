# Fast Network Simplex for Optimal Transport

## Intro

This library solves an optimal transport problem via linear programming, using the Network Simplex algorithm. 
This code is a simplification and optimization from the Network Simplex implementation present in the [LEMON 1.3.1 library](http://lemon.cs.elte.hu/pub/doc/latest-svn/a00783.html). 
It performs much faster, uses less memory, and only requires 2 header files that can directly be integrated into your project.

## Benchmark

### Protocol
I benchmarked the code performance against CPLEX's Network Simplex (function `CPXNETprimopt`), and a Transport Simplex implementation (from [Darren T. MacDonald](https://github.com/engine99/transport-simplex)).
The problem here is to compute the Earth Mover's Distance (and corresponding flow) between two histograms of N bins each (N is referred to as the *problem size*). The support of these histograms is a random sampling of the 2-D plane, and the histograms contain random values.
For the Transport Simplex, I've set the TSEPSILON to 1E-9 (the default value of 1E-6 results largely unoptimal solutions to relatively large problems).

### Speed
![Speed benchmark](https://raw.githubusercontent.com/nbonneel/network_simplex/master/doc/perf.png)

### Memory
![Memory benchmark](https://raw.githubusercontent.com/nbonneel/network_simplex/master/doc/memory.png)

### Accuracy
Graph coming soon. About 1E-6 % of error compared to CPLEX in double precision with floating point mode -fpfast.

### Observations

* The solver allows for solving problems of size 57000 in 6min 15s on my machine on 6 physical cores in double precision, or less than 2 min for problems of size 65000 in single precision. However, single precision incur significant errors for problems of size greater than 4000.
* For problems smaller than 1000, the overhead of multithreading makes the solver slower than the monothreaded version.
* CPLEX is faster for problems smaller than 200 approximately, but becomes orders of magnitude slower for larger problems (25x slower for problems of size 19000).
* The Transport Simplex has higher complexity, and is overall slowest
* However, my code becomes orders of magnitude slower on problems of odd sizes. The graph aboves is restricted to problems of even sizes, and a graph that includes some odd sizes problems can be found below:

![Performance benchmark](https://raw.githubusercontent.com/nbonneel/network_simplex/master/doc/perf_with_odd.png)
