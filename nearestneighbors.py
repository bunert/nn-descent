import numpy as np
import subprocess
import sklearn
import os
from timing import parse_output, Timingdata

class NearestNeighbors:
    # initialization with either a file
    # or 2d numpy array
    def __init__(self, NN=None, metric='l2', filename=None):
        if filename is None:
            self.NN = NN
        else:
            self.load(filename)

        self.N = self.NN.shape[0]
        self.K = self.NN.shape[1]

        self.metric = metric

    # calculates recall (assuming one of the datasets
    # represents the ground truth)
    def recall(self, other):
        if self.NN.shape != other.NN.shape:
            raise ValueError('dimensions of nearest neighbors dont match')

        if self.metric != other.metric:
            raise ValueError('metrics dont match')

        common_neighbors =list(map(lambda row1,row2:
                                   len(np.intersect1d(row1,row2)),
                                   self.NN,
                                   other.NN))

        return float(np.sum(common_neighbors))/(self.K*self.N)

    def load(self, filename):
        self.NN = np.loadtxt(filename)

# performs brute-force KNN on dataset
def nearest_neighbors(dataset, K, metric):
    if metric == 'l2':
        scikit_metric = 'minkowski'
        scikit_p = 2
    else:
        raise ValueError(metric + ' not implemented')

    # calculates K nearest neighbors for given dataset
    nbrs = sklearn.neighbors.NearestNeighbors(n_neighbors=K, algorithm='brute', metric=scikit_metric, p=scikit_p).fit(dataset.X)

    # gives back k nearest neighbors of training data
    # points can't be their own nn
    nbrs_indices = nbrs.kneighbors(return_distance=False)
    return NearestNeighbors(nbrs_indices, metric)

# performs reference knndescent on dataset
# returns nearestneighbors and timing
def c_nearest_neighbors(directory, dataset, K, metric, repetition):
    # calls reference implementation for NN


    if metric != 'l2':
        raise ValueError(metric + ' not implemented')
    dataset.save('data')

    path = os.path.join(directory, "a.out")

    cycles = np.zeros(repetition)
    runtime = np.zeros(repetition)
    nn_list = []

    for i in range(repetition):
        process = subprocess.run([path,'data','output', str(dataset.N), str(dataset.D), str(K)], check=True, stdout=subprocess.PIPE, universal_newlines=True)
        txt = process.stdout.splitlines()

        c,t  = parse_output(txt)
        nn_data = NearestNeighbors(filename='output')

        cycles[i] = c
        runtime[i] = t
        nn_list.append(nn_data)

    return nn_list, Timingdata(cycles,runtime, directory)
