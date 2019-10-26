# -*- coding: utf-8 -*-
import pandas

for N in [10, 15, 20, 25, 30, 40]:
    V = [[i, j] for i in range(N) for j in range(N)]
    pandas.DataFrame(V).to_csv(
        "../Instances/square_grid_{}X{}.dat".format(N, N), 
        sep=' ', 
        header=False, 
        index=True
    )
