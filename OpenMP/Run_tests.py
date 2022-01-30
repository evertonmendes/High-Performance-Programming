# This Python file uses the following encoding: utf-8
from subprocess import call
import os
import json
# print(os.listdir("GonzaloTest"))


def Run_Tests(n_attempts=50, type_seq_par="Sequential"):
    if type_seq_par == "Parallel":
        help_name = "_omp"
    else:
        help_name = ""

    data_dict = {test_type: {} for test_type in os.listdir("GonzaloTest")}

    for test_type in os.listdir("GonzaloTest"):
        for file in os.listdir("GonzaloTest/"+str(test_type)):
            for i in range(n_attempts):
                call("./FindEigen"+help_name+" "+file+" 0.00000000000001 "+file.replace(".dat", "")+"_out"+help_name+".dat",
                     shell=True)

    for test_type in os.listdir("GonzaloTest"):
        data_dict[test_type] = {file: []
                                for file in os.listdir("GonzaloTest/"+str(test_type))}
        for file in os.listdir("GonzaloTest/"+str(test_type)):
            if type_seq_par == "Parallel":
                data_dict[test_type][file] = [value for value in open(
                    'time_'+help_name.replace("_", "")+"_"+file)]
            else:
                data_dict[test_type][file] = [
                    value for value in open('time_'+help_name+file)]

    analysis_data = open(
        "data_analysis"+help_name+".json", 'w')
    json.dump(data_dict, analysis_data)
    analysis_data.close()


Run_Tests(n_attempts=50, type_seq_par="Sequential")
Run_Tests(n_attempts=50, type_seq_par="Parallel")
