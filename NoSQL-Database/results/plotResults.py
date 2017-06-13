import numpy as np
import matplotlib.pyplot as plt


def plotPerformance(mea, mod):
    if mea == 'Cache size':
        n = [100, 200, 300, 400]
        s = 'performanceCacheSize_'
    elif mea == 'Page size':
        n = [32, 64, 128, 256, 512, 1024]
        s = 'performancePageSize_'
    elif mea == 'MemTable size':
        n = [50, 100, 500, 1000, 1500, 2000]
        s = 'performanceMemTableSize_'

    if mod == 'w':
        xlabels = [500, 500, 500, 500]
    elif mod == 'r':
        xlabels = [500, 1000, 1500, 2000]

    wd = []
    rd = []
    for i in n:
        fn = s + str(i) + '.txt'
        with open(fn) as f:
            for l in f:
                ln = l.split(':')
                ld = ln[1].strip().split(',')
                if ln[0] == 'Time of write':
                    wd.append(ld[:-1])
                if ln[0] == 'Time of read':
                    rd.append(ld[:-1])

    if mod == 'rw':
        wid = 0.2
        wd = [w[0] for w in wd]
        plt.bar(range((len(n))), wd, wid, label = 'write')
        rd = [r[0] for r in rd]
        plt.bar(np.arange(len(n)) + wid, rd, wid, label='read')
        plt.legend()
        plt.ylabel('time/millisecond')
        plt.xlabel(mea)
        plt.xticks(range((len(n))), n)
        plt.show()
        return
    if mod == 'w':
        for i in range((len(n))):
            plt.plot(wd[i], label = mea + ": " + str(n[i]))
        plt.xlabel('write number')
    elif mod == 'r':
        for i in range((len(n))):
            plt.plot(rd[i], label = mea + ": " + str(n[i]))
        plt.xlabel('read number')

    plt.legend()
    plt.ylabel('time/millisecond')
    plt.xticks(range((len(n))), xlabels)
    plt.show()


if __name__ == '__main__':
    plotPerformance('Cache size', 'rw')
