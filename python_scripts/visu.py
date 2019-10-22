# -*- coding: utf-8 -*-
import os

import math

import pandas
import numpy as np

import matplotlib.pyplot as plt

def plot_instance(instance_path):
    V = pandas.read_csv(instance_path, header=None, sep=' ').dropna(axis=1).values[:, [1, 2]]
    
    circles = [plt.Circle((v[0], v[1]), 0.05, color='g') for v in V]
    
    fig, ax = plt.subplots()
    ax.set_xlim((-2, 2 + V[:, 0].max()))
    ax.set_ylim((-2, 2 + V[:, 1].max()))
    
    for circle in circles:
        ax.add_artist(circle)
    
    plt.title("Instance de {} cibles".format(len(V)))
    plt.show()

def build_graph(instance_path, R):
    V = pandas.read_csv(instance_path, sep=' ').dropna(axis=1).values[:, [1, 2]]
    G = np.array([[int(math.sqrt((v[0] - u[0])**2 + (v[1] - u[1])**2) <= R) for v in V] for u in V])
    return G


path = "../Instances/"
instances = os.listdir(path)
for instance in instances:
    print(instance)
    plot_instance(path + instance)


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