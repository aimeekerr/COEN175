# include <iostream>
#include "Type.h"
#include "Tree.h"
# include "generator.h"

using namespace std;

static ostream &operator<<(ostream &ostr, Expression *expr)
{
    expr->operand(ostr);
    return ostr;
}

void globalGenerator(const Scope *scope)
{
    int i;
    Symbols symbols = scope->symbols();
    cout << "#Global generator\n";
    int size = symbols.size();
    for(i=0;i<size;i++) 
    {
        if(!symbols[i]->type().isFunction()) 
        {
            cout << "\t.comm\t" << symbols[i]->name() << ", " << symbols[i]->type().size() << endl;
        }
    }
}

void Procedure::generate()
{
    unsigned i = 0;
    Symbols symbols = _body->declarations()->symbols();
    Parameters *p = _id->type().parameters();
    unsigned poffset = 8;
    unsigned doffset = 0;
    if(p != nullptr)
    {
        for(;i<p->size();i++)
        {
            symbols[i]->_offset = poffset;
            poffset += symbols[i]->type().size();
        }
    }
    for(;i<symbols.size();i++)
    {
        doffset -= symbols[i]->type().size();
        symbols[i]->_offset = doffset;
    }

    cout << _id->name() << ":" << endl;
    cout << "#prologue:" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp, %ebp" << endl;
    cout << "\tsubl\t$" << -doffset << ", %esp" << endl;
    _body->generate();
    cout << "#epilogue:" << endl;
    cout << "\tmovl\t%ebp, %esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl;
    cout << "\t.globl\t" << _id->name() << endl;
}

void Simple::generate() {
    cout << "#simple" << endl;
    _expr->generate();
}

void Block::generate() {
    cout << "#block" << endl;
    for(auto s : _stmts)
    {
        s->generate();
    }
}

void Assignment::generate() {
    cout << "#assignment" << endl;
    _right->generate();
    _left->generate();
    cout << "\tmovl\t" << _right << ", %eax" << endl;
    cout << "\tmovl\t%eax, " << _left << endl;
}

void Call::generate() {
    int i;
    cout << "#Call:" << endl;
    for(i=_args.size()-1;i>=0;i--)
    {
        _args[i]->generate();
        cout << "\tpush\t" << _args[i] << endl;
    }
    cout << "\tcall\t" << _expr << endl;
}

void Number::operand(ostream &ostr) const {
    cout << "\t$" << _value;
}

void Identifier::operand(ostream &ostr) const {
    if(_symbol->_offset == 0)
    {
        cout << _symbol->name();
    }
    else
    {
        cout << _symbol->_offset << "(%ebp)";
    }
}