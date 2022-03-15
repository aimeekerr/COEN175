#ifndef SCOPE_H
#define SCOPE_H
#include <ostream>
#include "Symbol.h"

typedef std::vector<Symbol *> Symbols;

class Scope {
    typedef std::string string;

    Scope *_enclosing;
    Symbols _symbols;

    public:
        Scope(Scope *enclosing = nullptr);

        Scope *enclosing() const { return _enclosing; };
        const Symbols &symbols() const { return _symbols; };

        Symbol *find(const string &name) const;
        Symbol *lookup(const string &name) const;
        void remove(const string &name);
        void insert(Symbol *symbol);
};

#endif