# -*- coding: utf-8 -*-
import os

import math

import pandas
import numpy as np

import scipy.spatial

import matplotlib.pyplot as plt


def read_solution(solution_path):
    V = pandas.read_csv(solution_path, header=None, sep=' ').dropna(axis=1).values[:,:]
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


def plot_solution(V, solution, r_capt, r_comm):
    fig, ax = plot_instance(V, False, "Solution avec {} capteurs pour {} cibles".format(len(solution), len(V)))

    circles = [plt.Circle((V[i, 0], V[i, 1]), r_capt, color='b', alpha=0.1) for i in solution]
    for circle in circles:
        ax.add_artist(circle)

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


def stupid_heuristic(V, r_comm, r_capt):
    # check the first node is (0, 0)
    assert V[0][0] + V[0][1] == 0
    G_capt = build_graph(V, r_capt)
    G_com = build_graph(V, r_comm)
    L = np.hstack([np.ones(1), np.zeros(len(V) - 1)]).reshape(-1, 1)
    captors = np.hstack([np.ones(1), np.zeros(len(V) - 1)]).reshape(-1, 1)
    while L.sum() < len(V):
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


def test_and_visualize_stupid_heuristic(instances):
    for instance in instances:
        print(instance)
        r_capt = 1
        r_comm = 2
        V = read_instance(path + instance)
        solution = stupid_heuristic(V, r_comm, r_capt)
        plot_solution(V, solution, r_capt, r_comm)


path = "../Instances/"
instances = os.listdir(path)


instance = "../Instances/captANOR900_15_20.dat"
V = read_instance(instance)
r_capt = 1
r_comm = 2
initial_solution = read_solution("../solutions/sol_1.txt")
final_solution = read_solution("../solutions/sol_2.txt")

plot_solution(V, initial_solution, r_capt, r_comm)

plot_solution(V, final_solution, r_capt, r_comm)







