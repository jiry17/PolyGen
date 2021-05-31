#include "config.h"
const std::string config::KSourcePath = SOURCEPATH;
const std::string config::KGrammarFilePath = config::KSourcePath + "basic/grammar.json";
const std::string config::KParserPath = config::KSourcePath + "/parser/python/main.py";
const std::string config::KEusolverPath = config::KSourcePath + "eusolver/";
const int config::KDefaultValue = 1000000000;
const int config::KIntMin = -15;
const int config::KIntMax = 15;
bool config::is_lifting_only = false;
int config::KMaxBranchNum = 5;
int config::KTermSolveTurns = 4000;
int config::KTermIntMax = 3;
int config::KTermConfidenceNum = 50;
int config::KRandomC = 10;
int config::KOrLimit = 2;
std::vector<int> global::int_consts;
double global::sample_time = 0.0;
int global::sample_num = 0;
std::map<std::string, int> global::sample_status;
int global::KSearchTimeLimit = -1;
timeval global::start_time;