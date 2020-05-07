import numpy as np
from pathlib import Path
import urllib.request
import pandas as pd


def get_dataset(data_name, n, dim):
    if data_name == 'gaussian':
        # n datapoints sampled from each of dim gaussians centered around canonical basis vector with dimension dim
        n = 1000 if n is None else n
        return GaussianDataset(dimension=dim, variance=2.0, n=n)
    elif data_name == 'audio':
        # Audio dataset as described in the NN-Descent publication
        #  54,387 points (192 dimensional)
        my_file = Path("audio.data")
        if not my_file.is_file():
            print("audio.data not here, downloading...")
            urllib.request.urlretrieve ("http://kluser.ch/audio.data", "audio.data")
        return AudioDataset(n)
    elif args.dataset == 'mnist' or args.dataset == 'digits' or args.dataset == 'sorted_mnist':
        # MNIST dataset of 70k handwritten digits (784 dimensional)
    
        mnist_filenames = ["mnist_train.csv","mnist_test.csv"]
    
        for csv_file in mnist_filenames:
            if not Path(csv_file).is_file():
                print("downloading " + csv_file)
                urllib.request.urlretrieve("https://pjreddie.com/media/files/" + csv_file, csv_file)

        if args.dataset == 'mnist' or args.dataset == 'digits':
            dataset = MnistDataset()
        elif args.dataset == 'sorted_mnist':
            dataset = MnistSortedDataset()
        else:
            #cannot occur
            print('error: dataset not implemented')

    else:
        print("dataset not supported")
        exit(1)


class Dataset:
    def __init__(self,X):
        self.X = X
        self.N = X.shape[0]
        self.D = X.shape[1]

    def save(self, filename):
        # save dataset to space separated values file
        np.savetxt(filename, self.X)

class GaussianDataset(Dataset):
    def __init__(self, dimension, variance, n):
        # generate n//dimension points from each of dimension many sperical gaussians
        # the gaussians are each centered around a canonical basisvector
        # n//dimension*dimension many points in total

        cov = variance * np.identity(dimension)
        X = []
        for i in range(dimension):
            mean = np.zeros(dimension)
            mean[i] = 1.0
            X.append(np.random.multivariate_normal(mean, cov, n//dimension))
        X = np.vstack(X)
        np.random.shuffle(X)
        Dataset.__init__(self, X)

class AudioDataset(Dataset):

    # Audio dataset as described in the publicaiton. 54,387 points (192 dimensional)
    def __init__(self, n=0):
        path = 'audio.data'
        # read data as specified here
        # http://lshkit.sourceforge.net/dc/d46/matrix_8h.html
        with open(path, "rb") as f:
            def read_uint(uint_bytes):
                return int.from_bytes(uint_bytes, signed=False, byteorder='little')

            elem_size = read_uint(f.read(4))
            size = read_uint(f.read(4))
            dim_elem_size = read_uint(f.read(4))

            # reads all 4-byte floats into flat array of dimension size*dim_elem_size
            X = np.frombuffer(np.array(f.read(4*size*dim_elem_size)), dtype=np.float32);

            X = X.reshape((size,dim_elem_size))

            # take not the whole dataset
            if (n!=0):
                X = X[:n,:]

            self.X = X

#            X = np.zeros((size,dim_elem_size))
#
#            for i in range(size):
#                for j in range(dim_elem_size):
#                    X[i,j] = np.array(f.read(4)).view(dtype=np.float32)



        Dataset.__init__(self, X)

class MnistDataset(Dataset):

    # Default MNIST Dataset 70k handwritten digits (784 dimensional)

    def __init__(self):
        train = 'mnist_train.csv' # 60k
        test = 'mnist_test.csv' # 10k

        train_df = pd.read_csv(train, header=None)
        test_df = pd.read_csv(test, header=None)

        complete_df = pd.concat([train_df, test_df], axis = 0)

        self.X = complete_df.iloc[:,1:].to_numpy(dtype='float32')
        self.N = complete_df.shape[0] # 70,000
        self.D = complete_df.shape[1] - 1 # 784


class MnistSortedDataset(Dataset):

    # MNIST dataset sorted according to a 1d umap

    def __init__(self):
        df = pd.read_csv('mnist_sort.csv', header=None)



        self.X = df.to_numpy(dtype='float32')
        self.N = df.shape[0] # 70,000
        self.D = df.shape[1] - 1 # 784



