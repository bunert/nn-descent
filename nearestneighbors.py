import numpy as np
import subprocess
import sklearn

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
def reference_nearest_neighbors(dataset, K, metric):
    # calls reference implementation for NN
    if metric != 'l2':
        raise ValueError(metric + ' not implemented')
    dataset.save('data')
    subprocess.run(['nn_descent/a.out','data','output', str(dataset.N), str(dataset.D), str(K)], capture_output=False)
    return NearestNeighbors(filename='output')
