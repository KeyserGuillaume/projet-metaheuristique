import os

import pandas

path = "../Instances_grilles_tronquees/"
instances = os.listdir(path)

for instance in instances:
    N = int(instance[6:8])
    V = [[i, j] for i in range(N) for j in range(N)]
    df = pandas.read_csv(path + instance, header=None, sep=' ', skiprows=2)
    df[3] = df[3].apply(lambda x: float(x.replace('(', '').replace(',', '')))
    df[4] = df[4].apply(lambda x: float(x.replace(')', '')))
    V_minus = df.values[:, [3, 4]]
    V_minus = [list(v) for v in V_minus]
    V = [v for v in V if v not in V_minus]
    pandas.DataFrame(V).to_csv(
        "../Instances/" + instance[:-4] + "_ok.dat",
        sep=' ',
        header=False,
        index=True
    )
