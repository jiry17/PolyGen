import random
from extra.config import *
from z3 import *

def getRandom():
    return random.randint(-KIntMin, KIntMax)

z3_semantics_map = {
    "=": lambda x: x[0] == x[1],
    "<": lambda x: x[0] < x[1],
    ">": lambda x: x[0] > x[1],
    "<=": lambda x: x[0] <= x[1],
    ">=": lambda x: x[0] >= x[1],
    "=>": lambda x: Implies(x[0], x[1]),
    "or": lambda x: Or(x[0], x[1]),
    "and": lambda x: And(x[0], x[1]),
    "not": lambda x: Not(x[0]),
    "+": lambda x: x[0] + x[1],
    "-": lambda x: x[0] - x[1],
    "*": lambda x: x[0] * x[1],
    "ite": lambda x: If(x[0], x[1], x[2])
}

def expr_to_z3(cons, param_map, func_name = None):
    if type(cons) == tuple:
        return cons[1]
    if type(cons) == str:
        assert cons in param_map
        return Int("Param" + str(param_map[cons]))
    if type(cons) == list:
        op = cons[0]
        if op == func_name:
            return Int("Result")
        sub_list = [expr_to_z3(sub_cons, param_map, func_name) for sub_cons in cons[1:]]
        print(cons, z3_semantics_map[op](sub_list))
        return z3_semantics_map[op](sub_list)
    assert False

_S = Solver()

def verify(program, param_map, cons, is_random = True):
    _S.push()
    _S.add(Not(And(cons)))
    _S.add(Int("Result") == expr_to_z3(program, param_map))
    # print(_S)
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