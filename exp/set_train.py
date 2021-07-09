import json
import os

def cp(file_1, file_2):
    with open(file_1, "r") as inp:
        lines = inp.readlines()
    with open(file_2, "w") as oup:
        oup.write("".join(list(map(lambda x: x.replace("Int2", "Int"), lines))))

def cp_with_truth(file_1, file_2, result):
    def get_param_num(expr):
        if type(expr) == str:
            if "Param" in expr: return int(expr[5:])
            return 0
        if type(expr) == tuple:
            return 0
        if type(expr) == list:
            if len(expr) == 2 and expr[0] in ["Int", "String", "Bool"]:
                return get_param_num((expr[0], expr[1]))
            ans = 0
            for sub_expr in expr:
                ans = max(ans, get_param_num(sub_expr))
            return ans
        assert False

    def expr_to_str(expr):
        if type(expr) == str:
            return expr
        if type(expr) == tuple:
            if expr[0] == "Bool":
                return "true" if expr[1] else "false"
            return str(expr[1])
        if type(expr) == list:
            if len(expr) == 2 and expr[0] in ["Int", "String", "Bool"]:
                return expr_to_str((expr[0], expr[1]))
            ans = [expr_to_str(sub_expr) for sub_expr in expr]
            return "(" + " ".join(ans) + ")"
        assert False
    param_num = get_param_num(result)
    expr = ["define-fun", "res", [["Param" + str(i), "Int"] for i in range(param_num + 1)], "Int", result]
    with open(file_1, "r") as inp:
        lines = inp.readlines()
    lines.append(expr_to_str(expr))
    with open(file_2, "w") as oup:
        oup.write("".join(list(map(lambda x: x.replace("Int2", "Int"), lines))))

if __name__ == "__main__":
    with open("result_cache/main_cegis.json", "r") as inp:
        result = json.load(inp)
    file_list = os.listdir("benchmark/full/")
    for file_name in file_list:
        if ".sl" not in file_name: continue
        benchmark_name = file_name.split("/")[-1]
        origin_name = "benchmark/full/" + file_name
        new_name = "benchmark/train/" + file_name
        if benchmark_name not in result:
            cp(origin_name, new_name)
        else:
            status = result[file_name]
            if status[0]["status"] == "fail":
                cp(origin_name, new_name)
            else:
                cp_with_truth(origin_name, new_name, status[0]["program"])