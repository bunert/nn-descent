import numpy as np
COV = [[1,0,0,0], [0,1,0,0], [0,0,1,0], [0,0,0,1]]

X = np.vstack((np.random.multivariate_normal((1,0,0,0), COV, 1000),
np.random.multivariate_normal((0,1,0,0), COV, 1000),
np.random.multivariate_normal((0,0,1,0), COV, 1000),
np.random.multivariate_normal((0,0,0,1), COV, 1000)))

np.savetxt("data", X)
