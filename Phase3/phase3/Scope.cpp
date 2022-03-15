#include "Scope.h"

using namespace std;

Scope::Scope(Scope *enclosing) : _enclosing(enclosing)
{
}

Symbol* Scope::find(const string &name) const
{
    size_t i;
    for(i=0;i<_symbols.size();i++)
    {
        if(_symbols[i]->name() == name)
        {
            return _symbols[i];
        }
    }
    return nullptr;
}

Symbol* Scope::lookup(const string &name) const
{
    Symbol *symbol = find(name);
    if(symbol != nullptr)
    {
        return symbol;
    }
    else if(_enclosing != nullptr)
    {
        return _enclosing->lookup(name);
    }
    else
    {
        return nullptr;
    }
}

void Scope::remove(const string &name)
{
    _symbols.pop_back();
}

void Scope::insert(Symbol *symbol)
{
    _symbols.push_back(symbol);
}