import numpy as np
import sklearn.neighbors
import subprocess
from dataset import GaussianDataset, AudioDataset
from nearestneighbors import reference_nearest_neighbors, nearest_neighbors

from pathlib import Path
import urllib.request

my_file = Path("audio.data")
if not my_file.is_file():
    print("audio.data not here, downloading...")
    urllib.request.urlretrieve ("http://kluser.ch/audio.data", "audio.data")

# dataset = GaussianDataset(10,10000,100)
dataset = AudioDataset()

print("Reference")
rnn = reference_nearest_neighbors(dataset, 20, 'l2')

print("True NN")
true_nn = nearest_neighbors(dataset, 20, 'l2')
print("Recall reference: ", true_nn.recall(rnn))
