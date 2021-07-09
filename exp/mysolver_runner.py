import os
import random
from util import verify, flush_line
from parser import sexp_from_string
from config import KMemoryLimit, KTimeLimit, KExampleLimit
import time
main_path = "../build/run"
ablation_type = 0

def change_ablation_type(typ):
    global ablation_type
    ablation_type = typ

def parse(file_name, run_type):
    if not os.path.exists(file_name): return None
    with open(file_name, "r") as inp:
        lines = inp.readlines()
    if run_type == "cegis" or run_type == "random":
        if len(lines) <= 1: return None
        example_num = int(lines[0][:-1])
        program = sexp_from_string("".join(lines[1:]))
        return example_num, program
    else:
        if len(lines) == 0: return None
        program = sexp_from_string("".join(lines))
        return program

def run_main_with_file(file_path, n, time_limit, run_type, oup_name=None):
    if oup_name is None:
        oup_name = str(random.randint(0, 10 ** 9)) + ".out"
    oup_file = "/tmp/" + oup_name
    #print("ablation type = " + str(ablation_type))
    if ablation_type > 0:
        command = ['ulimit -v ' + str(KMemoryLimit) + ';' + "timeout " + str(time_limit) + " " + main_path,
                file_path, oup_file, run_type, str(ablation_type), "> /tmp/oup 2> /tmp/err"]
    else:
        command = ['ulimit -v ' + str(KMemoryLimit) + ';' + "timeout " + str(time_limit) + " " + main_path,
                file_path, oup_file, run_type, "> /tmp/oup 2> /tmp/err"]
    command = " ".join(command)
    start_time = time.time()
    #print(command)
    os.system(command)
    end_time = time.time()
    result = parse(oup_file, run_type)
    param = {"Param" + str(i): i for i in range(n)}
    os.system("rm " + oup_file)
    return end_time - start_time, result, param

def run_main_cegis(ps_result, is_cegis = True):
    assert is_cegis
    param_num = ps_result["param_num"]
    bm = ps_result["bm"]
    name = str(random.randint(0, 10 ** 9))
    inp_name = name + ".in"
    oup_name = name + ".out"
    inp_path = "/tmp/" + inp_name
    with open(inp_path, "w") as oup:
        oup.write(bm)
    time_cost, result, param = run_main_with_file(inp_path, param_num, KTimeLimit, "cegis", oup_name)
    if result is None: return None
    return time_cost, result[0], result[1]

def run_main_with_example(ps_result, example_space, run_type):
    int_const_list = ps_result["int_const"]
    param_num = ps_result["param_num"]
    name = str(random.randint(0, 10 ** 9))
    inp_name = name + ".in"
    oup_name = name + ".out"
    inp_path = "/tmp/" + inp_name
    with open(inp_path, "w") as oup:
        oup.write(str(len(int_const_list)) + "\n")
        oup.write(" ".join(list(map(str, int_const_list))) + "\n")
        oup.write(str(param_num) + " " + str(len(example_space)) + "\n")
        for inp, o in example_space:
            oup.write(" ".join(list(map(str, inp))) + " " + str(o) + "\n")
    res = run_main_with_file(inp_path, param_num, KTimeLimit if run_type == "example" else KTimeLimit * 3, run_type, oup_name)
    os.system("rm " + inp_path)
    if res[1] is None: return None
    return res



def run_main_random(ps_result, is_cegis = False):
    if is_cegis: return run_main_cegis(ps_result)

    n = 1000
    g = ps_result["gen"]
    example_space = [g() for _ in range(n)]

    def evaluate(n, run_type):
        while len(example_space) < n: example_space.append(g())
        return run_main_with_example(ps_result, example_space[:n], run_type)

    def check(res, run_type):
        if res is None: return 0
        if run_type == "example":
            program, param = res[1], res[2]
        else:
            program, param = res[1][1], res[2]
        if verify(program, param, ps_result["cons"], False)[0] is None: return 1
        return 2

    n = 1
    final_result = []
    while True:
        flush_line()
        print("\rrun %d" % (n), end="")
        res = evaluate(n, "example")
        status = check(res, "example")
        if status == 1:
            final_result = res
            break
        if n == KExampleLimit: return None
        n = min(n * 2, KExampleLimit)

    l = n // 2
    r = n
    ans = n
    while l < r:
        mid = (l + r) // 2
        flush_line()
        print("\rrun %d" % (mid), end="")
        res = evaluate(mid, "example")
        status = check(res, "example")
        if status == 1:
            final_result = res
            ans = mid
            r = mid
        else:
            l = mid + 1
    return final_result[0], ans, final_result[1]