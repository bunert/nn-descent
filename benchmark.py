import numpy as np
import sklearn.neighbors
import os
import time
import subprocess
from dataset import get_dataset
from nearestneighbors import c_nearest_neighbors, py_nearest_neighbors, nearest_neighbors

from cost import Costdata, measure_costs
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-p','--path', required=True, help='path to a.out executable')
parser.add_argument('-r','--repetitions', help='repetitions', default=1, type=int)
parser.add_argument('-k', help='k', default=20, type=int)
parser.add_argument('-dim', help='dimension of space', default=100, type=int)
parser.add_argument('-m', '--metric', help='l2', default='l2')
parser.add_argument('-d', '--dataset', help='audio or gaussian', default='gaussian')
args = parser.parse_args()
print(args)

def save_data(fname, n, simi_evals, runtime_s, runtime_cycles, flops):
    X = np.array([n, simi_evals, runtime_s, runtime_cycles, flops]).transpose()
    np.savetxt(os.path.join('benchmarking', fname), X)

START = 8
END = 18
RESOLUTION = 1
inputs = []
cycles = []
runtimes = []
sim_evals = []

for n in np.logspace(START, END, num=RESOLUTION*(END-START+1), dtype=int, base=2):
    inputs.append(n)
    dataset = get_dataset(args.dataset, n, args.dim)

    nn_list, timing_data = c_nearest_neighbors(args.path, dataset, args.k, args.metric, args.repetitions)
    # append median or avg?
    cycles.append(timing_data.median_cycle)
    runtimes.append(timing_data.median_runtime)

    cost_data = measure_costs(args.path, dataset, args.k, args.metric)
    sim_evals.append(cost_data.metric_calls)

# for L2 norm:
# d operations for a[i]-b[i]
# d operations for squaring each component
# d-1 operations for summing up the squares
# 1 operation for sqrt
# so 3d flops per sim evaluation

flops = np.array(sim_evals)*dataset.D*3

save_data('{}_dim{}_logn{}to{}_k{}'.format(args.dataset,args.dim, START, END, args.k), inputs, sim_evals, runtimes, cycles, flops)

# plot_data(inputs,similarities, 'similarities')
