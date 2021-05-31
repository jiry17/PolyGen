#ifndef CPP_OPTIMIZED_UNIFIER_H
#define CPP_OPTIMIZED_UNIFIER_H

#include "unifier.h"

class OptimizedUnifier: public Unifier {
public:
    virtual Program* getCondition(const std::vector<PointExample*>& positive_example, const std::vector<PointExample*>& negative_example);
    virtual std::string getType() {return "Optimized";};
    //never called
    virtual Program* unify(const std::vector<PointExample*>& example_list, const std::vector<Program*>& term_list, const std::vector<Program*>& pred_list){assert(false);};
};


#endif //CPP_OPTIMIZED_UNIFIER_H
