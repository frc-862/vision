#ifndef MISCUTILS_H
#define MISCUTILS_H

#include <sstream>
#include <fstream>

std::string toString(int i);
std::string toString(float i);
bool fileExists(const std::string& name);

#endif // MISCUTILS_H
