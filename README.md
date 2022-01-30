# High-Performance-Programming
In Computational Physics, it is common the need for large-scale simulations as well as the analysis of large amounts of data. In such cases, program development must carefully consider performance issues, including making good use of existing computing resources and taking advantage of the many available forms of parallelism. This course will teach students how to develop programs taking into account performance on current architectures as well as relevant parallel programming techniques.

Important features of matrices, useful in many applications, are their eigenvalues and eigenvectors. There are methods that allow the precise calculation of all eigenvalues and eigenvectors of a matrix
given. Unfortunately, the computational cost is high, which makes the use of these methods unfeasible for
large matrices. But often we are only interested in the highest eigenvalue and corresponding
matrix eigenvector. These can be calculated approximately efficiently by methods
iterative, such as the power iteration.

The three folders are implementations of power iteration method.

- Sequential  
  - Wrong implementation due to bad use of pointers in vec = new_vec, problem fixed in Parallel folders

Implementation of the power iteration method using a linked list

- Parallel OpenMP

Implementation of the power iteration method using a linked list and parallelization. OpenMP is an application programming interface for multi-process shared memory programming on multiple platforms.

- Parallel MPI
  - The code needs to be changed to read the graph only by rank 0, and from there distributed to the other processors through MPI_scatterrv (for this it is necessary to create an MPI type that receives the nodes of a weighted linked list). Another problem in the code is the use of many unnecessary MPI_Barrier.


Implementation of the power iteration method using a linked list and parallelization. The Open MPI Project is an open source Message Passing Interface implementation that is developed and maintained by a consortium of academic, research, and industry partners. Open MPI is therefore able to combine the expertise, technologies, and resources from all across the High Performance Computing community in order to build the best MPI library available.

