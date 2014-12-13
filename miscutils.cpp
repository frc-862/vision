#include "miscutils.h"

std::string toString(int i) {
    std::ostringstream convert;
    convert << i;
    return convert.str();
}

std::string toString(double i) {
    std::ostringstream convert;
    convert << i;
    return convert.str();
}

bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}
