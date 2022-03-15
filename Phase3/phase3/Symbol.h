#ifndef SYMBOL_H
#define SYMBOL_H
#include "Type.h"

class Symbol {
    typedef std::string string;
    string _name;
    Type _type;

    public:
        Symbol(string name, const Type &type, bool defined = false);
        bool _defined;
        const string &name() const { return _name; };
        const Type &type() const { return _type; };
};

#endif