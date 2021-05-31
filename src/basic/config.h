#ifndef CPP_CONFIG_H
#define CPP_CONFIG_H

#include <algorithm>
#include <string>
#include <cmath>
#include <map>
#include <ctime>
#include <vector>
namespace config {
    extern const std::string KSourcePath;
    extern const std::string KGrammarFilePath;
    extern const std::string KEusolverPath;
    extern const std::string KParserPath;
    extern const int KDefaultValue;
    extern const int KIntMin;
    extern const int KIntMax;
    extern bool is_lifting_only;
    extern int KMaxBranchNum;
    extern int KTermSolveTurns;
    extern int KTermIntMax;
    extern int KTermConfidenceNum;
    extern int KRandomC;
    extern int KOrLimit;
}

namespace global {
    extern std::vector<int> int_consts;
    extern double sample_time;
    extern int sample_num;
    extern std::map<std::string, int> sample_status;
    extern int KSearchTimeLimit;
    extern timeval start_time;
}


#endif //CPP_CONFIG_H
