import numpy as np
import sys as sys

if len(sys.argv) < 1:
    print("Please specify a file name.")
    exit(1)

f = open(sys.argv[1], "r")
data = f.read().split('\n')
data = [x.strip() for x in data if x.strip() != '']
data = [x.split(' ')[2].strip() for x in data]
data = [float(x) / 1000 / 1000 for x in data]
mu = np.mean(data)
sigma = 1.96 * np.std(data) / np.sqrt(len(data))
print('%.2f ± %.2f MH/s' % (mu, sigma))
