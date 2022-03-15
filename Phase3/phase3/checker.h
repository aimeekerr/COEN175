#ifndef CHECKER_H
#define CHECKER_H
#include "Type.h"

typedef std::string string;

void openScope();
void closeScope();
void declareFunc(const string &name, const Type &type);
void defineFunc(const string &name, const Type &type);
void declareVariable(const string &name, const Type &type);
void openStruct(const string &name);
void closeStruct(const string &name);
void checkIfStructure(const string &name);
void checkID(const string &name);

#endif
