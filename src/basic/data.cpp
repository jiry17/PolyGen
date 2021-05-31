#include "data.h"
std::ostream & operator << (std::ostream& output,Data & c) {
    output << c.toString();
    return output;
}
std::ostream & operator << (std::ostream& output,DataList & c) {
    output << "(";
    for(int i = 0; i < (int)c.size(); i++)
        output << c[i] << (i == c.size()-1 ? "" :",");
    output <<")"; 
    return output;
}

std::ostream & operator << (std::ostream& output, PointExample & c) {
    output << c.first << " " << c.second;
    return output;
}