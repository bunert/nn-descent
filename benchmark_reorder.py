import numpy as np
import sklearn.neighbors
import re
import os
import time
import subprocess

from dataset import get_dataset
from nearestneighbors import c_nearest_neighbors
from benchmark import benchmark, benchmark_dim, git_clone
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-r','--repetitions', help='repetitions', default=1, type=int)
    parser.add_argument('-k', help='k', default=20, type=int)
    parser.add_argument('-dim', help='dimension of space', default=128, type=int)
    parser.add_argument('-m', '--metric', help='l2', default='l2')
    parser.add_argument('-ns', '--nstart', help='logn start', default=8, type=int)
    parser.add_argument('-ne', '--nend', help='logn end', default=18, type=int)
    parser.add_argument('-nr', '--nres', help='logn resolution', default=1, type=int)

    parser.add_argument('-ds', '--dimstart', help='dim start', default=8, type=int)
    parser.add_argument('-de', '--dimend', help='dim end', default=None, type=int)
    parser.add_argument('-dst', '--dimstep', help='dim step', default=8, type=int)

    parser.add_argument('-t', '--tag', help='single tag', default=None, type=str)

    parser.add_argument('-d', '--dataset', help='audio or gaussian', default='gaussian')
    args = parser.parse_args()

    print(args)
def extract_iteration_timing(s):
    pattern = 'Iteration \d: ([^ ]*) seconds'
    matches = re.findall(pattern, s)
    return matches


def speedup(n, clusters, dim):
    dataset = get_dataset(data_name='clustered', n=n, dim=dim, clusters=clusters, noshuffle=False)
    git_clone('blocked-distances-for-new-print-iterations')
    nn_list, no_reorder = c_nearest_neighbors('tmp/nn_descent', dataset, args.k, args.metric, args.repetitions)
    baseline = extract_iteration_timing(nn_list[0].stdout)

    git_clone('reorder-data')
    nn_list, reorder = c_nearest_neighbors('tmp/nn_descent', dataset, args.k, args.metric, args.repetitions)
    reordered = extract_iteration_timing(nn_list[0].stdout)

    return baseline, reordered, no_reorder.median_cycle/ reorder.median_cycle
result = []
baseline, reordered, speedup = speedup(n=2**15, clusters=16, dim=8)
print("speedup: ", speedup)
print("baseline iterations:  ", baseline)
print("reordered iterations: ", reordered)

#for n in np.arange(6,20):
#    for c in np.arange(0,n-5):
#        result.append((2**n,2**c, speedup(2**n, 2**c, 8)))
#        print(result)

# [(64, 1, 0.9559101899811993), (128, 1, 0.9690555480001507), (128, 2, 0.9832780346298928), (256, 1, 0.9828536233414283), (256, 2, 1.0002870041120466), (256, 4, 0.9701399607949969), (512, 1, 0.967343019016237), (512, 2, 0.994090390917653), (512, 4, 0.9699145666096312), (512, 8, 0.9717855923099248), (1024, 1, 0.9684759013661949), (1024, 2, 0.9789799775973117), (1024, 4, 1.003289260312607), (1024, 8, 0.9374308146284189), (1024, 16, 0.9906362653352014), (2048, 1, 0.9632803078709465), (2048, 2, 0.9633464505536057), (2048, 4, 0.9790344525008026), (2048, 8, 0.9472774251851179), (2048, 16, 0.9707321393142755), (2048, 32, 1.079004647286548), (4096, 1, 0.9609532881897748), (4096, 2, 0.9524254563680495), (4096, 4, 0.9671040565335829), (4096, 8, 0.9717580547344881), (4096, 16, 0.8777842138410796), (4096, 32, 1.015485151206543), (4096, 64, 0.9771056382130525), (8192, 1, 1.0101355385232005), (8192, 2, 0.940066775866804), (8192, 4, 0.9946986560565098), (8192, 8, 0.9845913073611057), (8192, 16, 1.0088608375390862), (8192, 32, 0.9216616969274612), (8192, 64, 1.054600929778897), (8192, 128, 1.0205750515934977), (16384, 1, 1.0097318203873753), (16384, 2, 1.032765007304959), (16384, 4, 1.104180035929592), (16384, 8, 1.160161265747127), (16384, 16, 1.0778678487794855), (16384, 32, 1.1568077940000232), (16384, 64, 1.1122574383739237), (16384, 128, 1.1292887090757064), (16384, 256, 1.1053933232473365), (32768, 1, 1.0348791301494074), (32768, 2, 1.0909801159005306), (32768, 4, 0.8188594152486289), (32768, 8, 1.265833099931236), (32768, 16, 1.2317485766326721), (32768, 32, 0.929711502501735), (32768, 64, 1.248637474247877), (32768, 128, 1.3304413156682267), (32768, 256, 1.131020342059264), (32768, 512, 1.1103537773547605), (65536, 1, 0.973614353243635), (65536, 2, 1.0286095436626022), (65536, 4, 1.1467837481176986), (65536, 8, 1.1813354571634007), (65536, 16, 1.2293305620413015), (65536, 32, 1.2508553123953743), (65536, 64, 1.28895895300997), (65536, 128, 1.2831028878563746), (65536, 256, 1.2226005859654843), (65536, 512, 1.1257221852658563), (65536, 1024, 1.0753632877839228), (131072, 1, 0.9904499940689733), (131072, 2, 0.9794945694670089), (131072, 4, 1.027652579615604), (131072, 8, 1.1785399517235027), (131072, 16, 1.422534497456563), (131072, 32, 1.3234032716189652), (131072, 64, 1.3323058653947868), (131072, 128, 1.3163966568248375), (131072, 256, 1.206096122030465), (131072, 512, 1.104767787848237), (131072, 1024, 1.0705037374878141), (131072, 2048, 1.0536931030276118), (262144, 1, 1.0275833043146247), (262144, 2, 1.0159379407861435), (262144, 4, 1.021120903475564), (262144, 8, 1.0991679336851066), (262144, 16, 1.174083765470199), (262144, 32, 1.271554217706958), (262144, 64, 1.4170665359567252), (262144, 128, 1.342992786099242), (262144, 256, 1.219851389006024), (262144, 512, 1.0787625850613383)]
