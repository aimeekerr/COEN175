#include <iostream>
#include <set>
#include "checker.h"
#include "Symbol.h"
#include "tokens.h"
#include "lexer.h"
#include "Scope.h"

using namespace std;

static Scope* current = nullptr;
static set<string> structs;

static string E1 = "redefinition of '%s'";
static string E2 = "conflicting types for '%s'";
static string E3 = "redeclaration of '%s'";
static string E4 = "'%s' undeclared";
static string E5 = "pointer type required for '%s'";
static string E6 = "'%s' has incomplete type";

void checkIfStructure(const string &name, const Type &type);

void openScope()
{
    cout << "Scope opening" << endl;
    current = new Scope(current);
    if(current == nullptr)
    {
        current = current;
    }
}

void closeScope()
{
    cout << "Closing scope" << endl;
    current = current->enclosing();
}

void declareFunc(const string &name, const Type &type)
{
    Symbol *symbol = current->find(name);

    if(symbol == nullptr)
    {
        symbol = new Symbol(name,type);
        current->insert(symbol);
    }
    else if(current->enclosing() != nullptr)
    {
        report(E3, name);
    }
    else if(symbol->type() != type)
    {
        report(E2, name);
    }
}

void defineFunc(const string &name, const Type &type)
{
    Symbol *symbol = current->find(name);

    if(symbol == nullptr) {
        symbol = new Symbol(name, type, true);
        current->insert(symbol);
    }
    else if(symbol->type().parameters() == nullptr)
    {
        current->remove(name);
        current->insert(new Symbol(name, type));
    }
    else if(symbol->type() != type) {
        report(E2, name);
    }
    else if(symbol->_defined) {
        report(E1, name);
    }
}

void declareVariable(const string &name, const Type &type)
{
    Symbol *symbol = current->find(name);

    if(symbol == nullptr)
    {
        checkIfStructure(name,type);
        symbol = new Symbol(name,type);
        current->insert(symbol);
    }
    else if(current->enclosing() != nullptr)
    {
        report(E3, name);
    }
    else if(type != symbol->type())
    {
        report(E2, name);
    }
}

void openStruct(const string &name)
{
    if(structs.count(name) > 0)
    {
        report(E1, name);
    }
    openScope();
}

void closeStruct(const string &name)
{
    closeScope();
    structs.insert(name);
}

void checkIfStructure(const string &name, const Type &type)
{
    if(type.isStruct() && type.indirection() == 0)
    {
        if(structs.count(type.specifier()) == 0)
            report(E6, name);
        else if(type.isCallBack())
        {
            report(E5, name);
        }
    }
}

void checkID(const string &name)
{
    Symbol *symbol = current->lookup(name);

    if(symbol == nullptr) 
    {
        report(E4, name);
    }
}