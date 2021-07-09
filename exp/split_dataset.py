import os
import pprint
import random

train_path = "benchmark/train"
batch_num = 3

def load_data_batch():
    file_list = list(filter(lambda x: ".sl" in x, os.listdir(train_path)))
    series_list = ["max1spos", "max", "array_search", "array_sum"]
    file_map = {}
    for file in file_list:
        name = file
        for series in series_list:
            if series in name:
                name = series
                break
        if name not in file_map:
            file_map[name] = []
        file_map[name].append(file)
    return file_map

if __name__ == "__main__":
    os.system("mkdir benchmark/cross")
    datas = list(load_data_batch().values())
    random.shuffle(datas)
    batch_list = [[] for _ in range(batch_num)]
    total = []
    for i, path_list in enumerate(datas):
        batch_list[i % batch_num].extend(path_list)
        total.extend(path_list)
    for id, path_list in enumerate(batch_list):
        new_dir = "benchmark/cross/" + str(id) + "/"
        os.system("rm -rf " + new_dir)
        os.system("mkdir " + new_dir)
        for file in total:
            if file in path_list: continue
            print(file)
            old_path = os.path.join(train_path, file)
            new_path = os.path.join(new_dir, file)
            os.system("cp " + old_path + " " + new_path)

for i in range(3):
    command = ["cd ~/2020S/euphony;", ". bin/setenv;"]
    train_command = "./bin/run_phog_learner /tmp/pycharm_project_180/benchmark/cross/{0}.model " \
                    "/tmp/pycharm_project_180/benchmark/cross/{0}/*.sl".format(str(i))
    command.append(train_command)
    os.system(" ".join(command))