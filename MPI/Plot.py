import pandas as pd
import os
import statistics as stats
import matplotlib.pyplot as plt
import numpy as np


def Analysis(type_seq_par="Sequential"):
    '''Create graphs of run time for each size file
    Args:
        type_seq_par, name of Solution. Availables: Sequential, Parallel_OMP, Parallel_MPI
    Return:
        None
    '''

    if type_seq_par == "Parallel_OMP":
        help_name = "_omp"
    elif type_seq_par== "Parallel_MPI":
        help_name = "_mpi"
    else:
        help_name = ""

    data_df = pd.read_json("My_Results/data_analysis"+help_name+".json")

    plt.rcParams['figure.figsize'] = [16, 18]

    mean_test_type = []
    std_test_type = []
    for test_type in os.listdir("GonzaloTest"):
        mean = []
        std = []
        files_into_analysis = []
        for file in os.listdir("GonzaloTest/"+str(test_type)):
            files_into_analysis.append(file)
            mean.append(stats.mean([float(value.replace("\n", ""))
                        for value in data_df[test_type][file]]))
            std.append(stats.stdev([float(value.replace("\n", ""))
                                    for value in data_df[test_type][file]]))

        print("mean: ", mean)
        print("stdev: ", std)

        mean_test_type.append(stats.mean(mean))
        std_test_type.append(sum(std))

        plt.title(test_type)
        plt.errorbar(np.arange(len(files_into_analysis)),
                     mean, yerr=std, ls='None', marker='o', label=test_type+" "+type_seq_par)
        ax = plt.gca()
        ax.xaxis.set_ticks(np.arange(len(files_into_analysis)))
        ax.xaxis.set_ticklabels(files_into_analysis, rotation=70)
        plt.xlabel("files")
        plt.ylabel("time(mean):seconds")
        plt.legend()
        plt.savefig(test_type+help_name+'.png')
        plt.clf()

    plt.title("all types")
    plt.xlabel("type of files")
    plt.ylabel("time(mean): seconds")
    plt.errorbar(np.arange(len(os.listdir("GonzaloTest"))), mean_test_type, yerr=std_test_type, label="all_types "+type_seq_par)
    ax = plt.gca()
    ax.xaxis.set_ticks(np.arange(len(os.listdir("GonzaloTest"))))
    ax.xaxis.set_ticklabels(os.listdir("GonzaloTest"), rotation=70)
    plt.legend()
    plt.savefig("all_types"+help_name+".png")
    plt.clf()
    

Analysis()
#Analysis("Parallel_OMP")
#Analysis("Parallel_MPI")

