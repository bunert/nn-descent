import numpy as np
import sklearn.neighbors

class Dataset:
    def __init__(self,X):
        self.X = X

    def save(self, filename):
        np.savetxt(filename, self.X)

class NearestNeighbors:
    def __init__(self, NN, metric):
        self.NN = NN
        self.N = NN.shape[0]
        self.K = NN.shape[1]

        self.metric = metric

    def load(self, filename):
        self.NN = np.loadtxt(filename)

def nearest_neighbors(dataset, K, metric):
    if metric == 'l2':
        scikit_metric = 'minkowski'
        scikit_p = 2
    else:
        raise ValueError(metric + ' not implemented')
    # calculates K nearest neighbors for given dataset
    nbrs = sklearn.neighbors.NearestNeighbors(n_neighbors=K, algorithm='brute', metric=scikit_metric, p=scikit_p).fit(dataset.X)

    # gives back k nn of training data, points can't be their own nn
    nbrs_indices = nbrs.kneighbors(return_distance=False)
    print(nbrs_indices)
    return NearestNeighbors(nbrs_indices, metric)


def recall(true_nn, approximation):
    if true_nn.NN.shape != approximation.NN.shape:
        raise ValueError('dimensions of nearest neighbors dont match')

    if true_nn.metric != approximation.metric:
        raise ValueError('metrics dont match')

    common_neighbors =list(map(lambda row1,row2: len(np.intersect1d(row1,row2)),true_nn.NN, approximation.NN))
    return float(np.sum(common_neighbors))/(true_nn.K*true_nn.N)


