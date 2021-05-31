#ifndef CPP_DECISION_TREE_UNIFIER_H
#define CPP_DECISION_TREE_UNIFIER_H

#include "unifier.h"

class DecisionTreeUnifier: public Unifier {
public:
    //never call this
    virtual Program* getCondition(const std::vector<PointExample*>& positive_example, const std::vector<PointExample*>& negative_example) {assert(false);};
    
    virtual Program* unify(const std::vector<PointExample*>& example_list,  
                            const std::vector<Program*>& term_list, const std::vector<Program*>& pred_list) ;
    virtual std::string getType() {return "DecisionTree";};
};


#endif //CPP_DECISION_TREE_UNIFIER_H
