import numpy as np

X = np.loadtxt("data")
print(np.linalg.norm(X[622,:]-X[3999,:]))
