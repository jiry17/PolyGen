from util import *
from config import KRepeatNum
from parser import benchmark_parser
from example_model import *
import json
import time
import os
def _collect_benchmark(folder):
    benchmark_list = os.listdir(folder)
    return [name for name in benchmark_list if ".sl" in name]

def run_cegis(parse_result, run_with_example):
    example_space = []
    while True:
        program, param = run_with_example(parse_result, example_space)
        inp, oup = verify(program, param, parse_result["cons"])
        # print(inp, oup, program)
        if inp is None: return program, len(example_space)
        example_space.append((inp, oup))

def _check_all_fail(result):
    if result is None:
        return True
    for info in result:
        if info["status"] != "fail": return False
    return True

def run(benchmark_folder, runner, name, clear_cache, skip_failed_cegis):
    print("test : " + name)
    cache_file = "result_cache/" + name + ".json"
    cache = load_cache(cache_file)
    random_and_skip = False
    if(name[-6:] == "random" and skip_failed_cegis):
        random_and_skip = True
        cegis_cache_file = "result_cache/" + name[:-6] + "cegis.json"
        cegis_cache = load_cache(cegis_cache_file)
        #print(cegis_cache_file)


    if cache is None: cache = {}
    if clear_cache: 
        cache = {}
    benchmark_list = reorder_benchmark(_collect_benchmark(benchmark_folder))
    is_changed = False
    #recal_list = ["qm_neg_5.sl", "qm_neg_eq_5.sl"]
    recal_list = []
    #benchmark_list = list(filter(lambda x: "array_search" in x[0] and x[1] == 13, benchmark_list))
    #print(KIntMin,KIntMax,KRepeatNum ,KTimeLimit ,KMemoryLimit ,KExampleLimit)
    for ind, (type_name, size, benchmark) in enumerate(benchmark_list):
        #if "mpg_example" in benchmark: continue
        #if "array_search" not in benchmark: continue
        print("run %d/%d: %s" % (ind, len(benchmark_list), benchmark))
        if benchmark not in recal_list and benchmark in cache:
            assert len(cache[benchmark]) == KRepeatNum
            continue
        cache[benchmark] = []
        if(random_and_skip and _check_all_fail(cegis_cache.get(benchmark))):
            for _ in range(KRepeatNum): cache[benchmark].append({"status": "fail"})
            print("skip for cegis fail")
            continue

        if (ind > 0 and benchmark_list[ind - 1][0] == type_name and _check_all_fail(cache[benchmark_list[ind - 1][2]])):
            for _ in range(KRepeatNum): cache[benchmark].append({"status": "fail"})
            print("skip for weaker benchmark fail")
            continue
        parse_result = benchmark_parser(os.path.join(benchmark_folder, benchmark))
        parse_result["gen"] = generate_random_example(get_input_generator(type_name, parse_result["param_num"], parse_result["bool_id"]), parse_result["oup"])
        parse_result["benchmark_name"] = benchmark

        init_res = runner(parse_result, True)
        print("finish init")
        print(init_res)
        if init_res is None:
            for _ in range(KRepeatNum): cache[benchmark].append({"status": "fail"})
            continue
        for _ in range(KRepeatNum):
            result = runner(parse_result)
            if result is None:
                cache[benchmark].append({"status": "fail"})
            else:
                print(result)
                time_cost, example_num, prog = result
                cache[benchmark].append({"status": "succeed", "example": example_num, "time": time_cost, "program": prog})
        for status in cache[benchmark]:
            if status["status"] == "fail":
                print(status["status"])
            else:
                print(status["status"], status["example"], status["time"])
        save_cache(cache_file, cache, not is_changed)
        is_changed = True
    save_cache(cache_file, cache, not is_changed)
    return cache

