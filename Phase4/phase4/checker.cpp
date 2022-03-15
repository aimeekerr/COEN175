/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <map>
# include <set>
# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static set<string> functions;
static map<string,Scope *> fields;
static Scope *outermost, *toplevel;
static const Type error;
static Scalar integer("int");

static string undeclared = "'%s' undeclared";
static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string incomplete = "'%s' has incomplete type";
static string nonpointer = "pointer type required for '%s'";

static string E1 = "invalid return type";
static string E2 = "invalid type for test expression";
static string E3 = "lvalue required in expression";
static string E4 = "invalid operands to binary %s";
static string E5 = "invalid operand to unary %s";
static string E6 = "invalid operand in cast expression";
static string E7 = "called object is not a function";
static string E8 = "invalid arguments to called function";
static string E9 = "using pointer to incomplete type";

# define isStructure(t) (t.isStruct() && t.indirection() == 0)


/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;

    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	openStruct
 *
 * Description:	Open a scope for a structure with the specified name.  If a
 *		structure with the same name is already defined, delete it.
 */

void openStruct(const string &name)
{
    if (fields.count(name) > 0) {
	delete fields[name];
	fields.erase(name);
	report(redefined, name);
    }

    openScope();
}


/*
 * Function:	closeStruct
 *
 * Description:	Close the scope for the structure with the specified name.
 */

void closeStruct(const string &name)
{
    fields[name] = closeScope();
}


/*
 * Function:	declareSymbol
 *
 * Description:	Declare a symbol with the specified NAME and TYPE.  Any
 *		erroneous redeclaration is discarded.  If a declaration has
 *		multiple errors, only the first error is reported.  To
 *		report multiple errors, remove the "return" statements and,
 *		if desired, the final "else".
 */

void declareSymbol(const string &name, const Type &type, bool isParameter)
{
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr)
	toplevel->insert(new Symbol(name, type));
    else if (toplevel != outermost) {
	report(redeclared, name);
	return;
    } else if (type != symbol->type()) {
	report(conflicting, name);
    	return;
    }

    if (isStructure(type)) {
	if (isParameter || type.isCallback() || type.isFunction())
	    report(nonpointer, name);
	else if (fields.count(type.specifier()) == 0)
	    report(incomplete, name);
    }
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.  In the case of multiple errors, only the
 *		first error is reported.  To report multiple errors, remove
 *		the "else"s.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (functions.count(name) > 0)
	report(redefined, name);
    else if (symbol != nullptr && type != symbol->type())
	report(conflicting, name);
    else if (isStructure(type))
	report(nonpointer, name);

    outermost->remove(name);
    delete symbol;

    symbol = new Symbol(name, type);
    outermost->insert(symbol);

    functions.insert(name);
    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}

Type checkReturn(const Type &left, const Type &t)
{
    if(left == error || t == error) 
    {
        return error;
    }
    if(left.isCompatiblewith(t)) 
    {
        return left;
    }
    report(E1);
    return error;
}

Type checkWhileForIf(const Type &left)
{
    if(left == error) 
    {
        return error;
    }
    else if(left.isValue()) 
    {
        return left;
    }
    report(E2);
    return error;
}

Type checkAssignment(const Type &left, const Type &right, bool &lvalue)
{
    if(left == error || right == error) 
    {
        return error;
    }
    if(!lvalue) 
    {
        report(E3);
        return error;
    }
    if(left.isCompatiblewith(right)) 
    {
        return left;
    }
    cout << left << " and " << right << endl;
    report(E4, "=");
    return error;
}

Type checkLogical(const Type &left, const Type &right, const string &op)
{
    if(left == error || right == error)
    {
        return error;
    }
    Type t1 = left.promote();
    Type t2 = right.promote();
    if(t1.isValue() && t2.isValue())
    {
        return integer;
    }
    report(E4, op);
    return error;
}

Type checkEquality(const Type &left, const Type &right, const string &op)
{
    if(left == error || right == error) 
    {
        return error;
    }
    Type t1 = left.promote();
    Type t2 = right.promote();
    if(t1.isCompatiblewith(t2)) 
    {
        return integer;
    }
    report(E4, op);
    return error;
}

Type checkRelational(const Type &left, const Type &right, const string &op)
{
    if(left == error || right == error) 
    {
        return error;
    }
    Type t1 = left.promote();
    Type t2 = right.promote();
    if(t1.isCompatiblewith(t2)) 
    {
        return integer;
    }
    report(E4, op);
    return error;
}

Type checkAdditive(const Type &left, const Type &right, const string &op)
{
    if(left == error || right == error)
    {
        return error;
    }
    Type t1 = left.promote();
    Type t2 = right.promote();
    if((t1.isStruct() && t1.isPointer() && fields.count(t1.specifier()) == 0 && t1.indirection() == 1) || (t2.isStruct() && t2.isPointer() && fields.count(t2.specifier()) == 0 && t2.indirection() == 1))
    {
        report(E9);
        return error;
    }
    if(t1 == integer && t2 == integer)
    {
        return integer;
    }
    if(t1.isPointer() && t2.isInteger())
    {
        return t1;
    }
    if(t1.isInteger() && t2.isPointer() && op == "+")
    {
        return t2;
    }
    if(t1.isPointer() && t2.isPointer() && op == "-" && t1 == t2)
    {
        return integer;
    }
    report(E4, op);
    return error;
}

Type checkMultiplicative(const Type &left, const Type &right, const string &op)
{
    if(left == error || right == error)
    {
        return error;
    }
    Type t1 = left.promote();
    Type t2 = right.promote();
    if(t1 == integer && t2 == integer)
    {
        return integer;
    }
    report(E4, op);
    return error;
}

Type checkNegate(const Type &right)
{
    if(right == error)
    {
        return error;
    }
    Type t1 = right.promote();
    if(t1.isInteger())
    {
        return integer;
    }
    report(E5,"-");
    return error;
}

Type checkNot(const Type &right)
{
    if(right == error)
    {
        return error;
    }
    if(right.isValue())
    {
        return integer;
    }
    report(E5,"!");
    return error;
}

Type checkAddress(const Type &right, const bool lvalue)
{
    if(right == error)
    {
        return error;
    }
    if(!lvalue)
    {
        report(E3);
        return error;
    }
    if(right.isCallback())
    {
        report(E5, "&");
        return error;
    }
    return Scalar(right.specifier(), right.indirection() + 1);
}

Type checkDereference(const Type &right)
{
    if(right == error)
    {
        return error;
    }
    Type t1 = right.promote();
    if(t1.isPointer() == false)
    {
        report(E5,"*");
        return error;
    }
    if(t1.isStruct() && fields.count(t1.specifier()) == 0)
    {
        report(E9);
        return error;
    }
    t1.indirection(t1.indirection() - 1);
    return t1;
}


Type checkSizeof(const Type &right)
{
    if(right == error)
    {
        return error;
    }
    if(right.isStruct() && fields.count(right.specifier()) == 0)
    {
        report(E9, "sizeof");
        return error;
    }
    if(right.isFunction())
    {
        report(E5, "sizeof");
        return error;
    }
    return integer;
}

Type checkTypecast(const Type &right, const string specifier, const unsigned indirection)
{
    if(right == error)
    {
        return error;
    }
    Type left = Scalar(specifier,indirection);
    Type t1 = right.promote();
    Type t2 = left.promote();
    if((t1.isInteger() && t2.isInteger()) || (t2.isPointer() && t1.isPointer()))
    {
        return left;
    }
    report(E6);
    return error;
}

Type checkArrayRef(const Type &left, const Type &ex)
{
    if(left == error || ex == error)
    {
        return error;
    }
    Type t1 = left.promote();
    Type t2 = ex.promote();
    if(!t1.isPointer() || !t2.isInteger())
    {
        report(E4,"[]");
        return error;
    }
    if(t1.isStruct() && fields.count(t2.specifier()) == 0)
    {
        report(E9);
        return error;
    }
    t1.indirection(t1.indirection() - 1);
    return t1;
}

Type checkFunctionCall(const Type &left, const Parameters &p)
{
    if(left == error)
    {
        return error;
    }
    if(!left.isFunction() && !left.isCallback())
    {
        report(E7);
        return error;
    }
    if (left.parameters() == nullptr) {
        return Scalar(left.specifier(), left.indirection());
    }
    if(left.parameters()->size() != p.size())
    {
        report(E8);
        return error;
    }
    for(unsigned i=0;i<p.size();i++)
    {
        Type p1 = left.parameters()->at(i).promote();
        Type p2 = p.at(i).promote();
        cout << "fails 2 " << p1 << " " << p2 << endl;
        if(!p1.isCompatiblewith(p2) || !p2.isValue())
        {
            report(E8);
            return error;
        }
    }
    return Scalar(left.specifier(),left.indirection());
}

Type checkDirectRef(const Type &left, const string id)
{
    if(!left.isStruct() || fields.count(left.specifier()) == 0)
    {
        report(E4,".");
        return error;
    }
    Symbol *symbol = fields[left.specifier()]->find(id);
    if (symbol == nullptr)
    {
        report(E4, ".");
        return error;
    }
    return symbol->type();
}

Type checkIndirectRef(const Type &left, const string id)
{
    Type t1 = left.promote();
    if(!t1.isPointer() || !left.isStruct())
    {
        report(E4,"->");
        return error;
    }
    if(fields.count(t1.specifier()) == 0)
    {
        report(E9);
        return error;
    }
    Symbol* symbol = fields[left.specifier()]->find(id);
    if(symbol == nullptr)
    {
        report(E4,"->");
        return error;
    }
    return symbol->type();
}