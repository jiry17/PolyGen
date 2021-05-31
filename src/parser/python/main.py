import sys
from sexp import sexp as sexpParser
from enum import Enum
import pprint as pp
import json
import pprint

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

def parse_file(bm_expr):
    signature = get_sublist(bm_expr, "synth-fun")
    assert len(signature) == 1
    signature = signature[0]
    param_info = []
    for name, param_type in signature[2]:
        param_info.append({"name": name, "type": param_type})
    func_name = signature[1]
    var_info = []
    var_map = {}
    for ind, (_, name, var_type) in enumerate(get_sublist(bm_expr, "declare-var")):
        var_info.append({"name": name, "type": var_type})
        var_map[name] = ind

    int_consts = []
    def collect_int_const(expr):
        if type(expr) == tuple:
            if expr[0] == "Int" and expr[1] not in int_consts:
                int_consts.append(expr[1])
            return
        if type(expr) == list:
            for sub_expr in expr: collect_int_const(sub_expr)
            return
        if type(expr) == str: return
        assert False

    for expr in bm_expr: collect_int_const(expr)

    def get_func_info(func_info):
        param_list = func_info[2]
        content = func_info[4]
        param_map = {param_name: ind for ind, (param_name, _) in enumerate(param_list)}
        def replace_param(c):
            if type(c) == str:
                if c in param_map: return ("Param", param_map[c])
                return c
            if type(c) == tuple:
                return c
            if type(c) == list:
                return [replace_param(sub_c) for sub_c in c]
            assert False
        return replace_param(content)

    client_func = {c[1]: get_func_info(c) for c in get_sublist(bm_expr, "define-fun")}

    def replace_with_param(c, param):
        if type(c) == tuple:
            if c[0] == "Param": return param[c[1]]
            return c
        if type(c) == str: return c
        if type(c) == list:
            return [replace_with_param(sub_c, param) for sub_c in c]
        assert False

    def constraint_to_json(cons):
        if type(cons) == tuple:
            return {"type": "const", "value_type": cons[0], "value": cons[1]}
        if type(cons) == str:
            if cons in client_func: return client_func[cons]
            assert cons in var_map
            return {"type": "var", "id": var_map[cons]}
        if type(cons) == list:
            op = cons[0]
            sub_list = [constraint_to_json(x) for x in cons[1:]]
            if op == func_name:
                return {"type": "func", "sub": sub_list}
            if op in client_func:
                param_list = cons[1:]
                return constraint_to_json(replace_with_param(client_func[op], param_list))
            return {"type": "op", "op": op, "sub": sub_list}
        assert False

    constraint_info = [constraint_to_json(cons[1]) for cons in get_sublist(bm_expr, "constraint")]
    return {"param": param_info, "var": var_info, "cons": constraint_info, "return": signature[3], "consts": int_consts}


if __name__ == "__main__":
    # python3 main.py file_name oup_file
    # sys.argv = [None, "test.sl", "test.json"]
    file_name = sys.argv[1]
    oup_file = sys.argv[2]
    bm_expr = sexp_from_file(file_name)
    # print(bm_expr)
    result_json = parse_file(bm_expr)
    with open(oup_file, "w") as oup:
        json.dump(result_json, oup)