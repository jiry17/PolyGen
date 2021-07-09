from z3 import *
import random

def default_generator(n, bool_list, int_min = -50, int_max = 50):
    def next():
        return [random.randint(int_min, int_max) if id not in bool_list else random.randint(0, 1) for id in range(n)]
    return next

def array_search_generator(n, int_min = -50, int_max = 50):
    def next():
        inp = []
        for i in range(n):
            while True:
                k = random.randint(int_min, int_max)
                if k in inp: continue
                inp.append(k)
                break
        inp = sorted(inp)
        pos = random.randint(0, n - 1)
        k = inp[pos]
        for i in range(pos + 1, n):
            inp[i - 1] = inp[i]
        inp[n - 1] = k
        return inp
    return next

def get_input_generator(type_name, param_num, bool_list):
    if "array_search" in type_name: return array_search_generator(param_num)
    return default_generator(param_num, bool_list)

def generate_random_example(input_generator, oup_calculator):
    def next():
        inp = input_generator()
        return [inp, oup_calculator(inp)]
    return next