# projet-metaheuristique

The c++ code contains some useless stuff :

- cost functions. They were intended to accelerate the algorithm, but this failed and now the implementation which is left of them is incomplete and basically doesn't do anything.
- the move musical_chairs : this move took quite some time in the making, and I've long thought it was actuelly useless and only deteriorated my performances. However, I recently found out that it could drastically improve results on instances such as the grid 40X40 with r_capt = r_comm = 2. It's probably only useful on a subset of the instances. Better keep it in the code.
- various methods of the class Field, like get_leaf which was intended to be used in the musical_chairs which careful evaluation would take some time, or get_triangle which became useless as soon as it was complete, and which in fact could be removed.

It's straightforward:
Target is a class for targets;
Field is a class for the instance;
LocalSearch is a class for the heuristic.

The python code contains the greedy heuristic, visualization stuff, some stuff in order to run the c++ program on all instances, and some more stuff involving gsheet API.
