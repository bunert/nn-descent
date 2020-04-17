import numpy as np
import sklearn.neighbors
import subprocess
from dataset import GaussianDataset, AudioDataset
from nearestneighbors import c_nearest_neighbors, nearest_neighbors
from pathlib import Path
import urllib.request
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-p','--path', help='path to a.out executable')
parser.add_argument('-r','--repetitions', help='repetitions', default=1, type=int)
parser.add_argument('-k', help='k', default=20, type=int)
parser.add_argument('-m', '--metric', help='l2', default='l2')
parser.add_argument('-d', '--dataset', help='audio or gaussian', default='gaussian')
args = parser.parse_args()
print(args)

dataset = {}

if args.dataset == 'gaussian':
    dataset = GaussianDataset(10,10000,100)
elif args.dataset == 'audio':
    my_file = Path("audio.data")
    if not my_file.is_file():
        print("audio.data not here, downloading...")
        urllib.request.urlretrieve ("http://kluser.ch/audio.data", "audio.data")
    dataset = AudioDataset()
else:
    print("dataset not supported")
    exit(1)

true_nn = nearest_neighbors(dataset, args.k, args.metric)
nn_list, timing_data = c_nearest_neighbors(args.path, dataset, args.k, args.metric, args.repetitions)
timing_data.print()
print("recall: ", list(map(true_nn.recall, nn_list)))
