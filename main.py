import numpy as np
import sklearn.neighbors
import subprocess
from dataset import GaussianDataset, AudioDataset
from nearestneighbors import c_nearest_neighbors, py_nearest_neighbors, nearest_neighbors
from pathlib import Path
import urllib.request
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-p','--path', required=True, help='path to a.out executable')
parser.add_argument('-r','--repetitions', help='repetitions', default=1, type=int)
parser.add_argument('-k', help='k', default=20, type=int)
parser.add_argument('-n', help='#points per gaussian', default=100, type=int) 
parser.add_argument('-m', '--metric', help='l2', default='l2')
parser.add_argument('-d', '--dataset', help='audio or gaussian', default='gaussian')
parser.add_argument('-v', '--verify', help='compare recall to pynndescent', action='store_true', dest='verify')
parser.set_defaults(verify=False)
args = parser.parse_args()
print(args)

dataset = {}

if args.dataset == 'gaussian':
    # 200 datapoints sampled from each of 10 gaussians centered around canonical basis vector
    dataset = GaussianDataset(dimension=10, variance=2.0, n=args.n)
elif args.dataset == 'audio':
    # Audio dataset as described in the NN-Descent publication
    #  54,387 points (192 dimensional)
    my_file = Path("audio.data")
    if not my_file.is_file():
        print("audio.data not here, downloading...")
        urllib.request.urlretrieve ("http://kluser.ch/audio.data", "audio.data")
    dataset = AudioDataset()
else:
    print("dataset not supported")
    exit(1)




nn_list, timing_data = c_nearest_neighbors(args.path, dataset, args.k, args.metric, args.repetitions)
timing_data.print()

if args.verify:
    true_nn = nearest_neighbors(dataset, args.k, args.metric)
    py_nn, py_timing_data = py_nearest_neighbors(dataset, args.k, args.metric, args.repetitions)
    py_timing_data.print()
    c_recall = list(map(true_nn.recall, nn_list))
    py_recall = list(map(true_nn.recall, py_nn))

    print("PyNNDescent: ", py_recall, ", ",args.path,": ",c_recall)
    print("Difference in mean: ", np.mean(py_recall)-np.mean(c_recall), " (lower is better)")
