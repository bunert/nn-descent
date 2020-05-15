import numpy as np
import sklearn.neighbors
import os
import time
import subprocess

from benchmark import benchmark

import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-r','--repetitions', help='repetitions', default=1, type=int)
    parser.add_argument('-k', help='k', default=20, type=int)
    parser.add_argument('-dim', help='dimension of space', default=100, type=int)
    parser.add_argument('-m', '--metric', help='l2', default='l2')
    parser.add_argument('-ns', '--nstart', help='logn start', default=8, type=int)
    parser.add_argument('-ne', '--nend', help='logn end', default=18, type=int)
    parser.add_argument('-nr', '--nres', help='logn resolution', default=1, type=int)
    parser.add_argument('-t', '--tag', help='single tag', default=None, type=str)

    parser.add_argument('-d', '--dataset', help='audio or gaussian', default='gaussian')
    args = parser.parse_args()

    print(args)

tags = ['T0', 'T1', 'T2', 'iterative_heapify', 'heap_insert_bounded', 'turbosampling', 'defered', 'l2intrinsics'] if args.tag is None else [args.tag]
for t in tags:
# delete temp directory
    subprocess.run(['rm', '-rf','tmp'])

# clone repo
    subprocess.run(['git', 'clone', 'git@gitlab.inf.ethz.ch:COURSE-ASL2020/team052.git', 'tmp'])

# set tag
    subprocess.run(['git', 'checkout', t], cwd='tmp')

    benchmark(args.dataset, args.dim, 'tmp/nn_descent', args.k, 'l2', args.repetitions, args.nstart, args.nend, args.nres, t)
