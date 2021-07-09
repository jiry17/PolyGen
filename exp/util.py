import random
from config import *
from z3 import *
import time, json

def getRandom():
    return random.randint(-KIntMin, KIntMax)

z3_semantics_map = {
    "=": lambda x: x[0] == x[1],
    "<": lambda x: x[0] < x[1],
    ">": lambda x: x[0] > x[1],
    "<=": lambda x: x[0] <= x[1],
    ">=": lambda x: x[0] >= x[1],
    "=>": lambda x: Implies(x[0], x[1]),
    "or": lambda x: Or(*x),
    "and": lambda x: And(*x),
    "not": lambda x: Not(x[0]),
    "+": lambda x: x[0] + x[1],
    "-": lambda x: (x[0] - x[1]) if len(x) > 1 else -x[0],
    "*": lambda x: x[0] * x[1],
    "ite": lambda x: If(x[0], x[1], x[2]),
    "let" : lambda x: x[0]
}
z3_type_map = {
    "=": "Bool",
    "<": "Bool",
    ">": "Bool",
    "<=": "Bool",
    ">=": "Bool",
    "=>": "Bool",
    "or": "Bool",
    "and": "Bool",
    "not": "Bool",
    "+": "Int",
    "-": "Int",
    "*": "Int"
}
from functools import reduce
value_semantics_map = {
    "=": lambda x: x[0] == x[1],
    "<": lambda x: x[0] < x[1],
    ">": lambda x: x[0] > x[1],
    "<=": lambda x: x[0] <= x[1],
    ">=": lambda x: x[0] >= x[1],
    "=>": lambda x: (not(x[0]) or x[1]),
    "or": lambda x: reduce(lambda a, b: a or b, x),
    "and": lambda x: reduce(lambda a, b: a and b, x),
    "not": lambda x: not(x[0]),
    "+": lambda x: x[0] + x[1],
    "-": lambda x: (x[0] - x[1]) if len(x) > 1 else -x[0],
    "*": lambda x: x[0] * x[1],
    "ite": lambda x: x[1] if x[0] else x[2],
    "let" : lambda x: x[0]
}
err_oup = open("log.out", "w")

def expr_to_z3(cons, param_map, func_name = None):
    #print(cons, type(cons))
    #print(param_map)
    if type(cons) == tuple:
        if cons[0] == "Int": return cons[1]
        if cons[0] == "Bool": return False if cons[1] == "false" else True
        assert False
    if type(cons) == str:
        assert cons in param_map
        if type(param_map[cons]) == int:
            return Int("Param" + str(param_map[cons]))
        elif len(param_map[cons]) == 2:
            para_id, para_type  = param_map[cons]
            if para_type == "Int":
                return Int("Param" + str(para_id))
            else:
                return Bool("Param" + str(para_id))
        elif len(param_map[cons]) == 3:
            return param_map[cons][2]
        else:
            assert False

    if type(cons) == list:
        op = cons[0]
        if op == "int" :
            if(type(cons[1])==str):
                return Int("Param" + str(param_map[cons[1]]))
            else: return cons[1][1]
        if op == func_name:
            return Int("Result")
        if op == "let":
            var,val = cons[1][0][0],cons[1][0][1]
            var_type = z3_type_map[val[0]]
            #print("var =" +str(var))
            #print("val =" +str(val) +" vartype =" + var_type)
            new_param_map = param_map
            new_param_map[var] = [len(new_param_map),var_type]
            val = expr_to_z3(val, new_param_map, func_name)
            new_param_map[var] = [len(new_param_map),var_type , val]
            #print(new_param_map)

            sub_list = [expr_to_z3(sub_cons, new_param_map, func_name) for sub_cons in cons[2:]]
 
            
        else:
            sub_list = [expr_to_z3(sub_cons, param_map, func_name) for sub_cons in cons[1:]]
        err_oup.write(str(cons) + "\n")
        err_oup.write(str(sub_list) + "\n")
        #if op == "or" or op =="and":
        #    print(op)
        #    print(sub_list)
        #print()
        #print(op, sub_list)

        return z3_semantics_map[op](sub_list)
    assert False

_S = Solver()

import sys

def expr_to_val(cons, param_map):
    if type(cons) == tuple:
        if cons[0] == "Int": return cons[1]
        if cons[0] == "Bool": return False if cons[1] == "false" else True
        assert False
    if type(cons) == str:
        assert cons in param_map
        return param_map[cons]
    if type(cons) == list:
        op = cons[0]
        if op == "let":
            var,val = cons[1][0][0],cons[1][0][1]
            new_param_map = param_map
            val = expr_to_val(val, new_param_map)
            new_param_map[var] = val
            #print(new_param_map)
            sub_list = [expr_to_val(sub_cons, new_param_map) for sub_cons in cons[2:]]
        else:
            sub_list = [expr_to_val(sub_cons, param_map) for sub_cons in cons[1:]]
        return value_semantics_map[op](sub_list)
    assert False

def verify_by_example(program, param_map, example_space):
    sys.setrecursionlimit(10000000) 
    z3expr = expr_to_val(program, param_map)
    for point in example_space:
        for var,val in zip(param_map,point[0]):
            param_map[var] = val
        myoup = expr_to_val(program, param_map)
        if point[1] == myoup : 
            #print("success")
            continue
        #print("fail")
        inp,oup = point[0], point[1]
        break
    return inp,oup

def verify(program, param_map, cons, is_random = True):
    #print(program)
    #print(param_map)
    #print(cons)
    #input()
    _S.push()
    _S.add(Not(And(cons)))
    _S.add(Int("Result") == expr_to_z3(program, param_map))
    #print(program)
    #print(expr_to_z3(program, param_map))
    #input()
    # print(_S)
    
    sys.setrecursionlimit(10000000)    
    if _S.check() == unsat:
        _S.pop()
        return None, None
    model = _S.model()
    if is_random:
        id_list = list(range(len(param_map)))
        random.shuffle(id_list)
        for id in id_list:
            param = Int("Param" + str(id))
            bound = getRandom()
            if random.randint(0, 1) == 0:
                _S.add(param >= bound)
            else: _S.add(param <= bound)
            if _S.check() == unsat: break
            model = _S.model()
    _S.pop()
    inp = [parse_int_variable(model, Int("Param" + str(id))) for id in range(len(param_map))]
    _S.push()
    _S.add(And(cons))
    for id in range(len(param_map)):
        _S.add(Int("Param" + str(id)) == inp[id])
    assert _S.check() == sat
    oup = parse_int_variable(_S.model(), Int("Result"))
    _S.pop()
    return inp, oup


def parse_int_variable(model, var):
    try:
        return model.eval(var).as_long()
    except:
        return getRandom()

def load_cache(cache_path):
    if os.path.exists(cache_path):
        with open(cache_path, "r") as inp:
            cache = json.load(inp)
        return cache
    return None

def save_cache(cache_path, cache, is_backup = False):
    if os.path.exists(cache_path) and is_backup:
        t = time.strftime("%Y-%m-%d-%H:%M:%S", time.localtime())
        os.system("mv " + cache_path + " " + cache_path + t)
    with open(cache_path, "w") as oup:
        json.dump(cache, oup)

def _get_last_int(benchmark_name):
    result = ""
    while len(benchmark_name) > 0 and ord(benchmark_name[-1]) in range(ord('0'), ord('9') + 1):
        result = benchmark_name[-1:] + result
        benchmark_name = benchmark_name[:-1]
    if len(result) == 0: return 0
    return int(result)

def _prefix(name_1, name_2):
    return name_2[:len(name_1)] == name_1

def _get_benchmark_info(benchmark_name):
    if ".sl" in benchmark_name: benchmark_name = benchmark_name[:-3]
    if _prefix("array_sum", benchmark_name):
        num = int(benchmark_name.split('_')[-1])
        size = int(benchmark_name.split('_')[-2])
        return "array_sum" + str(num), size
    if _prefix("max", benchmark_name) or _prefix("array_search", benchmark_name):
        size = _get_last_int(benchmark_name)
        return benchmark_name[:-len(str(size))], size
    return benchmark_name, 0

def reorder_benchmark(benchmark_list):
    info_list = []
    for benchmark_name in benchmark_list:
        type_name, ind = _get_benchmark_info(benchmark_name)
        info_list.append((type_name, ind, benchmark_name))
    return sorted(info_list)


def flush_line():
    print("\r" + " " * 50, end="")