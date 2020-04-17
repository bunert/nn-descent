import numpy as np

class Dataset:
    def __init__(self,X):
        self.X = X
        self.N = X.shape[0]
        self.D = X.shape[1]

    def save(self, filename):
        np.savetxt(filename, self.X)

class GaussianDataset(Dataset):
    def __init__(self, dimension, variance, n):
        cov = variance * np.identity(dimension)
        X = []
        for i in range(dimension):
            mean = np.zeros(dimension)
            mean[i] = 1.0
            X.append(np.random.multivariate_normal(mean, cov, n))
        X = np.vstack(X)
        Dataset.__init__(self, X)

class AudioDataset(Dataset):
    def __init__(self):
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
            X = np.frombuffer(np.array(f.read(4*size*dim_elem_size)), dtype=np.float32)

            X = X.reshape((size,dim_elem_size))
            self.X = X

#            X = np.zeros((size,dim_elem_size))
#
#            for i in range(size):
#                for j in range(dim_elem_size):
#                    X[i,j] = np.array(f.read(4)).view(dtype=np.float32)



        Dataset.__init__(self, X)


