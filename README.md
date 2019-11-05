# projet-metaheuristique

The c++ code contains some useless stuff :

- cost functions. They were intended to accelerate the algorithm, but this failed and now the implementation which is left of them is incomplete and basically doesn't do anything.
- the move musical_chairs : I am not clear whether it is useless or not, but I generally think it is not very useful. Still, more experiments are needed before deciding whether to remove the code or leave.
- various methods of the class Field, like get_leaf which was intended to be used in the musical_chairs which careful evaluation would take some time, or get_triangle which became useless as soon as it was complete, and which in fact could be removed.

It's straightforward:
Target is a class for targets;
Field is a class for the instance;
LocalSearch is a class for the heuristic.

The python code contains the greedy heuristic, visualization stuff, some stuff in order to run the c++ program on all instances, and some more stuff involving gsheet API.
