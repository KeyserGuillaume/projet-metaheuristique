# projet-metaheuristique

This is a project done during my master. The goal was to design an efficient heuristic for solving the minimum connected dominating set problem.

## Problem description

There are N targets, which are 2D points and two floating point values: Rcom <= Rcapt.  A feasible solution of the problem is a subset C of the N points verifying:
- Each target is within distance Rcapt of a target in C;
- Between any two targets in C, there is a path formed by targets in C where each successive targets are within distance Rcom of each other.

Assuming being in C means that a captor is positioned on the target, the first condition ensures every target is monitored, and the second condition ensures that the captors communicate with each other.

We minimize the number of elements in C.

## Algorithm employed

We coded a Local Search algorithm which yields very good solutions on all instances. Captors are moved in their local neighborhood so that both feasibility conditions are verified at all time. Several captors can be moved at once, sometimes over large distances (one move focuses on moving captors whose only use is to keep all captors connected). Before moving a captor, we check it still serves a purpose, and this is the mechanism used to improve the solution. We spend a lot of time shifting between equivalent solutions.

## Notes on the code

The c++ code contains some useless stuff :

- cost functions. They were intended to accelerate the algorithm, but this failed and now the implementation which is left of them is incomplete and basically doesn't do anything.
- the move musical_chairs : this move took quite some time in the making, and I've long thought it was actuelly useless and only deteriorated my performances. However, I recently found out that it could drastically improve results on instances such as the grid 40X40 with r_capt = r_comm = 2. It's probably only useful on a subset of the instances. Better keep it in the code.
- various methods of the class Field, like get_leaf which was intended to be used in the musical_chairs, or get_triangle which became useless as soon as it was complete, and which in fact could be removed.

It's straightforward:
Target is a class for targets;
Field is a class for the instance;
LocalSearch is a class for the heuristic.

The python code contains the greedy heuristic, visualization stuff, some stuff in order to run the c++ program on all instances, and some more stuff involving gsheet API which helped me benchmark the algorithms.
