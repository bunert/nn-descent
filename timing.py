import numpy as np
import subprocess
import sklearn
import os
from dataset import GaussianDataset, AudioDataset

class Timingdata:
    def __init__(self,c, t, method):
        self.method = method
        self.cycles = c
        self.median_cycle = np.median(c)
        self.avg_cycle = np.average(c)
        self.std_cycle = np.std(c)
        self.var_cycle = np.var(c)

        self.runtime = t
        self.median_runtime = np.median(t)
        self.avg_runtime = np.average(t)
        self.std_runtime = np.std(t)
        self.var_runtime = np.var(t)


    def save(self, filename):
        np.savetxt(filename, self.X)



def get_timing(method, dataset, K, metric):
    if metric != 'l2':
        raise ValueError(metric + ' not implemented')

    dataset.save('data')
    path = method
    path = os.path.join(method, "a.out")
    # 'nn_descent/a.out'
    # with open('out.txt','w+') as fout:
    #     with open('err.txt','w+') as ferr:
    process = subprocess.run([path,'data','output', str(dataset.N), str(dataset.D), str(K)], check=True, stdout=subprocess.PIPE, universal_newlines=True)
    txt = process.stdout.splitlines()
    return parse_output(txt)

    # return NearestNeighbors(filename='output')

def parse_output(txt):
    if (txt[0].split()[-1] != 'finished'):
        print("Algorithm not finished, probably some error occured.")
    c = float(txt[1].split()[0])
    t = float(txt[2].split()[0])
    return c,t

def print_timing(data):
    print("Performance measurement: "+ data.method + " (using " + str(data.cycles.shape[0]) + " repetitions)")
    print("\ncycles:")
    print('{:<20s}{:<10s}'.format("average:", str(data.avg_cycle)))
    print('{:<20s}{:<10s}'.format("median:", str(data.median_cycle)))
    print('{:<20s}{:<10s}'.format("std deviation:", str(data.std_cycle)))
    print('{:<20s}{:<10s}'.format("variance:", str(data.var_cycle)))

    print("\nruntime (in seconds):")
    print('{:<20s}{:<10s}'.format("average:",str(data.avg_runtime)))
    print('{:<20s}{:<10s}'.format("median:" ,str(data.median_runtime)))
    print('{:<20s}{:<10s}'.format("std deviation:", str(data.std_runtime)))
    print('{:<20s}{:<10s}'.format("variance:", str(data.var_runtime)))

def rep_timing(repetition, method, dataset, K, metric):
    cycles = np.zeros(repetition)
    runtime = np.zeros(repetition)

    for i in range(repetition):
        c, t = get_timing(method, dataset, K, metric)
        cycles[i] = c
        runtime[i] = t

    return Timingdata(cycles,runtime, method)



dataset = GaussianDataset(10,10000,100)
timing_data = rep_timing(20, "nn_descent", dataset, 20, 'l2')
print_timing(timing_data)
