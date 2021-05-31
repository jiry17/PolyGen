//
// Created by pro on 2021/2/6.
//

#include "parser.h"
#include "config.h"
#include "semantics_factory.h"
#include "util.h"

int main() {
    std::string file_name = config::KSourcePath + "/benchmark/CLIA_Track/fg_max2.sl";
    auto* spec = parser::parseSpecification(file_name);
    auto* param_1 = new Program({}, new ParamSemantics(0, TINT));
    auto* param_2 = new Program({}, new ParamSemantics(1, TINT));
    auto* cmp = new Program({param_1, param_2}, semantics::getSemanticsFromName("<"));
    auto* p = new Program({cmp, param_2, param_1}, semantics::getSemanticsFromName("ite"));

    std::cout << p->toString() << std::endl;

    auto* counter_example = spec->verify(p, true);
    if (counter_example) {
        std::cout << "Fail" << std::endl;
        std::cout << util::dataList2String(counter_example->first) << " => " << counter_example->second.toString() << std::endl;
    } else {
        std::cout << "Pass" << std::endl;
    }
}