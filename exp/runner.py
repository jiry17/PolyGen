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

    if cache is None: cache = {}
    if clear_cache: 
        cache = {}
    benchmark_list = reorder_benchmark(_collect_benchmark(benchmark_folder))
    is_changed = False
    for ind, (type_name, size, benchmark) in enumerate(benchmark_list):
        print("run %d/%d: %s" % (ind, len(benchmark_list), benchmark))
        if benchmark in cache:
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

