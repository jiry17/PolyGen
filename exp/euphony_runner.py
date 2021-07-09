import os
import random
from parser import sexp_from_string
from config import KMemoryLimit, KTimeLimit, KExampleLimit
import time
from util import verify, flush_line
euphony_path = "../recommend/euphony"
model_path = "../benchmark/cross/"
def parse_result(result_file):
    if not os.path.exists(result_file): return None
    with open(result_file, "r") as inp:
        oup_lines = inp.readlines()
    if len(oup_lines) <= 1: return None
    example_num = int(oup_lines[0][:-1])
    program = sexp_from_string("".join(oup_lines[1:]))
    var_map = {name: i for i, (name, _) in enumerate(program[2])}
    program = program[4]
    return example_num, program, var_map

def get_model_path(benchmark_name):
    print("get model path", benchmark_name)
    file_pos = None
    for i in range(3):
        train_set = os.path.join(model_path, str(i) + "/" + benchmark_name)
        if not os.path.exists(train_set):
            assert file_pos is None
            file_pos = i
    assert file_pos is not None
    pos = os.path.join(model_path, str(file_pos) + ".model")
    return pos


def run_euphony_with_file(file_path, benchmark_name, oup_name=None):
    if oup_name is None:
        oup_name = str(random.randint(0, 10 ** 9)) + ".out"
    oup_file = "/tmp/" + oup_name
    model_pos = get_model_path(benchmark_name)
    command = ["cd", euphony_path, ";", ". bin/setenv;", 'ulimit -v ' + str(KMemoryLimit) + ';' + "timeout " + str(KTimeLimit),
               "bin/run_with_new_phog", model_pos, file_path, ">", oup_file]
    command = " ".join(command)
    print(command)
    start_time = time.time()
    os.system(command)
    #exit(0)
    end_time = time.time()
    result = parse_result(oup_file)
    os.system("rm " + oup_file)
    return end_time - start_time, result

def run_euphony_cegis(ps_result, is_cegis=True):
    assert is_cegis
    bm = ps_result["bm"]
    name = str(random.randint(0, 10 ** 9))
    inp_name = name + ".in"
    oup_name = name + ".out"
    inp_path = "/tmp/" + inp_name
    with open(inp_path, "w") as oup:
        oup.write(bm)
    time_cost, result = run_euphony_with_file(inp_path, ps_result["benchmark_name"], oup_name)
    os.system("rm " + inp_path)
    if result is None: return None
    return time_cost, result[0], result[1]

def run_euphony_with_example(p_res, example_space):
    name = str(random.randint(0, 10 ** 9))
    inp_name = name + ".in"
    oup_name = name + ".out"
    inp_path = "/tmp/" + inp_name
    with open(inp_path, "w") as oup:
        oup.write(p_res["builder"](example_space) + "\n")
    time_cost, result = run_euphony_with_file(inp_path, p_res["benchmark_name"], oup_name)
    os.system("rm " + inp_path)
    if result is None: return None
    return time_cost, result[0], result[1], result[2]

def run_euphony_random(ps_result, is_cegis = False):
    if is_cegis: return run_euphony_cegis(ps_result)
    l = 1
    r = 1
    example_space = []
    g = ps_result["gen"]

    def evaluate(n):
        while len(example_space) < n: example_space.append(g())
        return run_euphony_with_example(ps_result, example_space[:n])

    def check(res):
        if res is None: return 0
        program, param = res[2], res[3]
        if verify(program, param, ps_result["cons"], False)[0] is None: return 1
        return 2
    while True:
        flush_line()
        print("\rtest %d" % (r), end="")
        res = evaluate(r)
        status = check(res)
        if status == 1: break
        if r == KExampleLimit or status == 0: return None
        l = r + 1
        r = min(r * 2, KExampleLimit)

    ans = r
    while l < r:
        flush_line()
        print("\rsearch %d %d" % (l, r), end="")
        mid = (l + r) // 2
        current_res = evaluate(mid)
        status = check(current_res)
        if status == 1:
            r = mid
            ans = mid
            res = current_res
        else:
            l = mid + 1
    return res[0], ans, res[2]

