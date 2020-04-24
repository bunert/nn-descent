# Nearest Neighbour Descent

This is the repository for the Advanced Systems Lab project by:
- Jonas Bokstaller
- Tobias Buner
- Dan Kluser
- Samuel Rutz

The goal of the project is to implement NN-Descent which is an efficient
algorithm for approximate K-Nearest Neighbour Graph (K-NNG) construction.

The algorithm is described in the pulication "Efficient K-Nearest Neighbor
Graph Construction for Generic Similarity Measures" by Wei Dong et al.

Useful links:
 - Publication: https://www.cs.princeton.edu/cass/papers/www11.pdf
 - Project System: https://medellin.inf.ethz.ch/courses/263-2300-ETH/
 - PyNNDescent: https://github.com/lmcinnes/pynndescent

## Installation

After cloning the repository compile the C-code in the directory 'nn_descent' with the following command:

'''
gcc  -O3 -ffast-math -march=native -o a.out knnd.c knnd_test.c vec.c -lm
'''

Additionally you will have to create a [Python virtual environment](https://docs.python.org/3/tutorial/venv.html).
To do so you can follow these steps:

1. create the environment in the chosen path (e.g ./pip-env)
    '''
    python -m venv ./pip-env
    '''
2. activate the environment 'source pip-env/bin/activate'
3. install the requirements listed in requirements.txt
    'pip install -r requirements.txt'