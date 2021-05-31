//
// Created by pro on 2020/12/2.
//

#ifndef CPP_RECORDER_H
#define CPP_RECORDER_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <sys/time.h>

class Recorder {
    std::vector<std::string> stage_stack;
    std::map<std::string, double> time_cost;
    std::map<std::string, int> info;
    timeval t;
    std::string name;
public:
    Recorder() {
        gettimeofday(&t, nullptr);
    }
    void setName(std::string _name) {name = _name;}
    void pushStage(std::string stage_name);
    void popStage();
    void record(std::string name, int value);
    void addRecord(std::string name, int value);
    std::string getTop() {return stage_stack[stage_stack.size() - 1];}
    ~Recorder();
};

namespace recorder {
    extern Recorder recorder;
}


#endif //CPP_RECORDER_H
