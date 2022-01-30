# This Python file uses the following encoding: utf-8
from subprocess import call
import os
import json
# print(os.listdir("GonzaloTest"))


def Run_Tests(n_attempts=50, type_seq_par="Sequential", np=2):
    '''Run All test files given a type of Solution: Sequential, Parallel with OpenMP or Parallel with MPI(Message Passing Interface).
    At the end, Create a json file with time records
    Args:
        n_attempts, number of attempts for each test file
        type_seq_par, name of Solution. Availables: Sequential, Parallel_OMP, Parallel_MPI
        np, number of processors for MPI only, the other solutions use all available processors
    Return:
        None 
    '''
    if type_seq_par == "Parallel_OMP":
        help_name = "_omp"
    elif type_seq_par == "Parallel_MPI":
        help_name = "_mpi"
    else:
        help_name = ""

    data_dict = {test_type: {} for test_type in os.listdir("GonzaloTest")}
    #+"/GonzaloTest/"+test_type+"/"

    for test_type in os.listdir("GonzaloTest"):
        for file in os.listdir("GonzaloTest/"+str(test_type)):
            for i in range(n_attempts):
                if type_seq_par=="Sequential" or type_seq_par=="Parallel_OMP":
                    call("./FindEigen"+help_name+" "+file+" 0.00000000000001 "+file.replace(".dat", "")+"_out"+help_name+".dat",
                        shell=True)
                else:
                    call("mpirun -np "+str(np)+" --allow-run-as-root ./FindEigen"+help_name+" "+file+" 0.00000000000001 "+file.replace(".dat", "")+"_out"+help_name+".dat",
                        shell=True)

    for test_type in os.listdir("GonzaloTest"):
        data_dict[test_type] = {file: []
                                for file in os.listdir("GonzaloTest/"+str(test_type))}
        for file in os.listdir("GonzaloTest/"+str(test_type)):
            if type_seq_par == "Parallel_OMP" or type_seq_par == "Parallel_MPI":
                data_dict[test_type][file] = [value for value in open(
                    'time_'+help_name.replace("_", "")+"_"+file)]
            else:
                data_dict[test_type][file] = [
                    value for value in open('time_'+help_name+file)]

    analysis_data = open("data_analysis"+help_name+".json", 'w')
    json.dump(data_dict, analysis_data)
    analysis_data.close()


#Run_Tests(n_attempts=50, type_seq_par="Sequential")
#Run_Tests(n_attempts=50, type_seq_par="Parallel_OMP")
Run_Tests(n_attempts=50, type_seq_par="Parallel_MPI", np=2)
