from sexp import sexp as sexpParser
from util import expr_to_z3, parse_int_variable
from example_model import *
import os
from z3 import *
import pprint as pp

def strip_comments(bm_file):
    no_comments = '('
    for line in bm_file:
        line = line.split(';', 1)[0]
        no_comments += line
    return no_comments + ')'

def sexp_from_string(value):
    return sexpParser.parseString(value, parseAll = True).asList()[0]

def sexp_from_file(benchmarkFileName):
    try:
        benchmarkFile = open(benchmarkFileName)
    except:
        return None
    bm = strip_comments(benchmarkFile)
    bm_expr = sexp_from_string(bm)
    benchmarkFile.close()
    return bm_expr

def get_sublist(bm_expr, name):
    return list(filter(lambda x: x[0] == name, bm_expr))

def benchmark_parser(file_name):
    bm_expr = sexp_from_file(file_name)
    constraint_list = get_sublist(bm_expr, "constraint")
    var_list = get_sublist(bm_expr, "declare-var")
    var_name_list = []
    bool_list = []
    for ind, (_, var_name, var_type) in enumerate(var_list):
        assert var_type == "Int" or var_type == "Int2"
        var_name_list.append(var_name)
        if var_type == "Int2": bool_list.append(ind)
    signature = get_sublist(bm_expr, "synth-fun")
    assert len(signature) == 1
    signature = signature[0]
    func_name = signature[1]
    assert signature[3] == "Int"
    pp.pprint

    func_map = {}
    for _, name, params, return_type, body in get_sublist(bm_expr, "define-fun"):
        func_param_map = {var_name: ind for ind, (var_name, _) in enumerate(params)}
        def formalize_body(expr):
            if type(expr) == str:
                if expr in func_param_map:
                    return ("Param", func_param_map[expr])
                return expr
            if type(expr) == tuple: return expr
            if type(expr) == list:
                return [formalize_body(sub_expr) for sub_expr in expr]
            assert False
        func_map[name] = formalize_body(body)

    def replace_func_call(expr, params):
        if type(expr) == str: return expr
        if type(expr) == tuple:
            if expr[0] == "Param": return params[expr[1]]
            return expr
        if type(expr) == list:
            return [replace_func_call(sub_expr, params) for sub_expr in expr]
        assert False

    def remove_func_call(expr):
        if type(expr) == str:
            if expr in func_map: return func_map[expr]
            return expr
        if type(expr) == list:
            op = expr[0]
            if op in func_map:
                return remove_func_call(replace_func_call(func_map[op], expr[1:]))
            return [remove_func_call(sub_expr) for sub_expr in expr]
        if type(expr) == tuple:
            return expr
        assert False
    constraint_list = [remove_func_call(cons) for cons in constraint_list]

    def get_func_call(constraint):
        if type(constraint) == list:
            op = constraint[0]
            if op == func_name:
                return constraint
            result = None
            for sub_constraint in constraint:
                sub_result = get_func_call(sub_constraint)
                if sub_result is not None:
                    if result is None: result = sub_result
                    assert result == sub_result
            return result
        return None
    func_call = None
    for constraint in constraint_list:
        res = get_func_call(constraint)
        assert res is not None
        if func_call is None: func_call = res
        assert func_call == res

    param_list = func_call[1:]
    var_map = {}
    for i in range(len(param_list)):
        assert type(param_list[i]) == str and param_list[i] in var_name_list
        var_map[param_list[i]] = i
        for j in range(i):
            assert param_list[i] != param_list[j]
    #assert len(param_list) == len(var_name_list)

    S = Solver()

    z3_list = []
    for constraint in constraint_list:
        z3_list.append(expr_to_z3(constraint[1], var_map, func_name))
    S.add(And(z3_list))

    def get_output(inp):
        assert len(inp) == len(var_name_list)
        S.push()
        for i, value in enumerate(inp):
            S.add(Int("Param" + str(i)) == value)
        assert S.check() == sat
        model = S.model()
        S.pop()
        return parse_int_variable(model, Int("Result"))

    int_const_list = []
    def collect_int_cons(expr):
        if type(expr) == tuple:
            if expr[0] == "Int" and expr[1] not in int_const_list:
                int_const_list.append(expr[1])
            return
        if type(expr) == list:
            for sub_expr in expr: collect_int_cons(sub_expr)
            return
        if type(expr) == str: return
        assert False
    collect_int_cons(bm_expr)

    param_list = [name for name, _ in signature[2]]
    start_rules = param_list
    for int_const in int_const_list: start_rules.append(("Int", int_const))
    start_rules.append(["+", "Start", "Start"])
    start_rules.append(["-", "Start", "Start"])
    start_rules.append(["ite", "StartBool", "Start", "Start"])
    bool_rules = [[op, "StartBool", "StartBool"] for op in ["and", "or"]]
    bool_rules.append(["not", "StartBool"])
    bool_rules.extend([[op, "Start", "Start"] for op in ["<=", "=", ">="]])
    signature[4] = [["Start", "Int", start_rules], ["StartBool", "Bool", bool_rules]]
    def term_to_str(term):
        if type(term) == str: return term
        if type(term) == tuple: return str(term[1])
        if type(term) == list: return "(" + " ".join([term_to_str(sub_term) for sub_term in term]) + ")"
        assert False

    def replaced_int2(expr):
        if type(expr) == str:
            if expr == "Int2": return "Int"
            return expr
        if type(expr) == list:
            return [replaced_int2(sub_expr) for sub_expr in expr]
        if type(expr) == tuple:
            return expr
        assert False

    build_head = replaced_int2([["set-logic", "LIA"], signature])
    def build_sygus_file(example_list):
        file = build_head.copy()
        for inp, oup in example_list:
            call = [func_name] + [("Int", value) for value in inp]
            file.append(["constraint", ["=", call, ("Int", oup)]])
        file.append(["check-synth"])
        return "\n".join([term_to_str(term) for term in file])

    formalized_expr = [["set-logic", "LIA"], signature]
    formalized_expr.extend(get_sublist(bm_expr, "define-fun"))
    formalized_expr.extend(get_sublist(bm_expr, "declare-var"))
    formalized_expr.extend(get_sublist(bm_expr, "constraint"))
    formalized_expr.append(["check-synth"])
    formalized_expr = replaced_int2(formalized_expr)

    return {"oup": get_output, "int_const": int_const_list, "builder": build_sygus_file, "cons": z3_list,
            "param_num": len(var_name_list), "bm": "\n".join([term_to_str(term) for term in formalized_expr]),
            "bool_id": bool_list}