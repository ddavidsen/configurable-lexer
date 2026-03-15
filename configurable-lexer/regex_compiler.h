#ifndef REGEX_COMPILER_H
#define REGEX_COMPILER_H

#include <string>

using namespace std;

class NFA;

string InfixToPostfix(const string& infix);
NFA PostfixtoNFA(const string& postfix);

#endif