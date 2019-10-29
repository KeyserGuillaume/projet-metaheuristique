# -*- coding: utf-8 -*-
import os

import pandas
import numpy as np

import scipy.spatial

import matplotlib.pyplot as plt

from google_sheet_api import write_results_on_gsheet
from call_cpp_program import call_cpp_program


def read_solution(solution_path):
    V = pandas.read_csv(solution_path, header=None, sep=' ').dropna(axis=1).values[:, 0]
    return V


def read_instance(instance_path):
    V = pandas.read_csv(instance_path, header=None, sep=' ').dropna(axis=1).values[:, [1, 2]]
    return V


def plot_instance(V, show=True, title=None):
    circles = [plt.Circle((v[0], v[1]), 0.05, color='g') for v in V]
    
    fig, ax = plt.subplots()
    ax.set_xlim((-2, 2 + V[:, 0].max()))
    ax.set_ylim((-2, 2 + V[:, 1].max()))
    
    for circle in circles:
        ax.add_artist(circle)

    if title is None:
        title = "Instance de {} cibles".format(len(V))
    plt.title(title)

    if show:
        plt.show()

    return fig, ax


def plot_solution(V, solution, r_capt, r_comm, plot_comm=False):
    fig, ax = plot_instance(V, False, "Solution avec {} capteurs pour {} cibles".format(len(solution), len(V)))

    circles = [plt.Circle((V[i, 0], V[i, 1]), r_capt, color='b', alpha=0.1) for i in solution]
    for circle in circles:
        ax.add_artist(circle)

    if plot_comm:
        G_comm = build_graph(V, r_comm)
        for u in solution:
            for v in solution:
                if u != v and G_comm[u, v] == 1:
                    ax.add_artist(plt.Line2D((V[u, 0], V[v, 0]), (V[u, 1], V[v, 1]), c='k'))

    plt.show()


def build_graph(V, R):
    # G = np.array([[int(math.sqrt((v[0] - u[0])**2 + (v[1] - u[1])**2) <= R) for v in V] for u in V])
    G = (scipy.spatial.distance_matrix(V, V) <= R).astype(int)
    return G


def plot_all_instances(instances):
    for instance in instances:
        print(instance)
        plot_instance(path + instance)


def compute_instances_statistics(instances):
    for instance in instances:
        print(instance)
        for R in [1, 2, 3]:
            print(R)
            G = build_graph(path + instance, R)
            print("min", G.sum(axis=0).min())
            print("max", G.sum(axis=0).max())
            print("mean", G.sum(axis=0).mean())
            print("var", G.sum(axis=0).var())
            print("\n")
        print("\n")


def stupid_heuristic(V, r_capt, r_comm):
    # check the first node is (0, 0)
    assert V[0][0] + V[0][1] == 0
    G_capt = build_graph(V, r_capt)
    G_com = build_graph(V, r_comm)
    L = np.hstack([np.ones(1), np.zeros(len(V) - 1)]).reshape(-1, 1)
    captors = np.hstack([np.ones(1), np.zeros(len(V) - 1)]).reshape(-1, 1)
    # i = 0
    while L.sum() < len(V):  # and i < 1000:
        # i += 1
        print("{}% couverts".format(L.sum()/len(V)*100))
        v_com = (np.dot(G_com, captors) >= 1).astype(int)
        # we could set 0 for captors already there but since those will cover 0 new
        # targets they cannot be picked.
        A = v_com*G_capt*(1 - L.T)
        v_star = np.argmax(A.sum(axis=1))
        L += A[v_star, :].reshape(-1, 1)
        captors[v_star] = 1
    solution = [i for i in range(1, len(captors)) if captors[i] == 1]
    print("{}% couverts".format(L.sum()/len(V)*100))
    print("On a construit une solution avec {} capteurs".format(len(solution)))
    return solution


def test_and_visualize_stupid_heuristic(instances, r_capt=1, r_comm=1, show_comm=True):
    for instance in instances:
        print(instance)
        V = read_instance(path + instance)
        solution = stupid_heuristic(V, r_capt, r_comm)
        plot_solution(V, solution, r_capt, r_comm, show_comm)


path = "../Instances/"
instances = os.listdir(path)
instances.sort()
radius_pairs = [[1, 1], [1, 2], [2, 2], [2, 3]]
# this defines 4*24 = 96 instances of the problem...

my_list = []
for instance in instances:
    for radius_pair in radius_pairs:
        my_list += [instance + "_{}_{}".format(radius_pair[0], radius_pair[1])]
write_results_on_gsheet("A", my_list)

my_list = []
for instance in instances:
    for radius_pair in radius_pairs:
        print(instance, radius_pair)
        my_list += [len(stupid_heuristic(read_instance(path + instance),
                                         radius_pair[0], radius_pair[1]))]
write_results_on_gsheet("B", my_list)

my_list = []
for instance in instances:
    for radius_pair in radius_pairs:
        print(instance, radius_pair)
        my_list += [call_cpp_program(instance, radius_pair[0], radius_pair[1])]

write_results_on_gsheet("E", my_list)

# instance = "../Instances/captANOR225_9_20.dat"
instance = "../Instances/captANOR1500_15_100.dat"
# instance = "../Instances/square_grid_15X15.dat"
V = read_instance(instance)
solution = read_solution("../solutions/sol_1.txt")
plot_solution(V, solution, 1, 1, True)
test_and_visualize_stupid_heuristic([instance], 2, 2, True)

