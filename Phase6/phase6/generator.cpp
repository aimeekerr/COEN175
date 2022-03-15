/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <cassert>
# include <map>
# include "generator.h"

static int offset;
static string funcname;
static ostream &operator <<(ostream &ostr, Expression *expr);

static Register *eax = new Register("%eax", "%al");
static Register *ecx = new Register("%ecx", "%cl");
static Register *edx = new Register("%edx", "%dl");

static vector<Register *> registers = {eax, ecx, edx};
static map<string, Label> strings;

/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
	return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	operator << (private)
 *
 * Description:	Convenience function for writing the operand of an
 *		expression using the output stream operator.
 */

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    if (expr->_register != nullptr)
	    return ostr << expr->_register;

    expr->operand(ostr);
    return ostr;
}


/*
 * Function:	Expression::operand
 *
 * Description:	Write an expression as an operand to the specified stream.
 */

void Expression::operand(ostream &ostr) const
{
    ostr << _offset << "(%ebp)";
}


/*
 * Function:	Identifier::operand
 *
 * Description:	Write an identifier as an operand to the specified stream.
 */

void Identifier::operand(ostream &ostr) const
{
    if (_symbol->_offset == 0)
	    ostr << global_prefix << _symbol->name();
    else
	    ostr << _symbol->_offset << "(%ebp)";
}


/*
 * Function:	Number::operand
 *
 * Description:	Write a number as an operand to the specified stream.
 */

void Number::operand(ostream &ostr) const
{
    ostr << "$" << _value;
}


/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression.
 *
 * 		On a 32-bit Linux platform, the stack needs to be aligned
 * 		on a 4-byte boundary.  (Online documentation seems to
 * 		suggest that 16 bytes are required, but 4 bytes seems to
 * 		work and is much easier.)  Since all arguments are 4-bytes
 *		wide, the stack will always be aligned.
 *
 *		On a 32-bit OS X platform, the stack needs to aligned on a
 *		16-byte boundary.  So, if the stack will not be aligned
 *		after pushing the arguments, we first adjust the stack
 *		pointer.  However, this trick only works if none of the
 *		arguments are themselves function calls.
 *
 *		To handle nested function calls, we need to generate code
 *		for the nested calls first, which requires us to save their
 *		results and then push them on the stack later.  For
 *		efficiency, we only first generate code for the nested
 *		calls, but generate code for ordinary arguments in place.
 */

void Call::generate()
{
    unsigned numBytes;


    /* Generate code for any nested function calls first. */

    numBytes = 0;

    for (int i = _args.size() - 1; i >= 0; i --) {
	    numBytes += _args[i]->type().size();

        if (STACK_ALIGNMENT != SIZEOF_REG && _args[i]->_hasCall)
            _args[i]->generate();
    }


    /* Align the stack if necessary. */

    if (align(numBytes) != 0) {
        cout << "\tsubl\t$" << align(numBytes) << ", %esp" << endl;
        numBytes += align(numBytes);
    }


    /* Generate code for any remaining arguments and push them on the stack. */

    for (int i = _args.size() - 1; i >= 0; i --) {
        if (STACK_ALIGNMENT == SIZEOF_REG || !_args[i]->_hasCall)
            _args[i]->generate();

        cout << "\tpushl\t" << _args[i] << endl;
        assign(_args[i], nullptr);
    }


    /* Call the function and then reclaim the stack space. */

    load(nullptr, eax);
    load(nullptr, ecx);
    load(nullptr, edx);

    if (_expr->type().isCallback()) {
	    _expr->generate();

        if (_expr->_register == nullptr)
            load(_expr, getreg());

        cout << "\tcall\t*" << _expr << endl;
        assign(_expr, nullptr);

    } else
	    cout << "\tcall\t" << _expr << endl;

    if (numBytes > 0)
	    cout << "\taddl\t$" << numBytes << ", %esp" << endl;

    assign(this, eax);
}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (auto stmt : _stmts) {
	    stmt->generate();
    }
}


/*
 * Function:	Simple::generate
 *
 * Description:	Generate code for a simple (expression) statement, which
 *		means simply generating code for the expression.
 */

void Simple::generate()
{
    _expr->generate();
    assign(_expr, nullptr);
}


/*
 * Function:	Procedure::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Procedure::generate()
{
    int param_offset;


    /* Assign offsets to the parameters and local variables. */

    param_offset = 2 * SIZEOF_REG;
    offset = param_offset;
    allocate(offset);


    /* Generate our prologue. */

    funcname = _id->name();
    cout << global_prefix << funcname << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp, %ebp" << endl;
    cout << "\tsubl\t$" << funcname << ".size, %esp" << endl;


    /* Generate the body of this function. */

    _body->generate();


    /* Generate our epilogue. */

    cout << endl << global_prefix << funcname << ".exit:" << endl;
    cout << "\tmovl\t%ebp, %esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl << endl;

    offset -= align(offset - param_offset);
    cout << "\t.set\t" << funcname << ".size, " << -offset << endl;
    cout << "\t.globl\t" << global_prefix << funcname << endl << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();

    for (auto symbol : symbols)
        if (!symbol->type().isFunction()) 
        {
            cout << "\t.comm\t" << global_prefix << symbol->name() << ", ";
            cout << symbol->type().size() << endl;
        }
    cout << "\t.data\t" << endl;
    for (auto s : strings) 
    {
        string first = escapeString(s.first);
        cout << s.second << ":\t.asciz\t\"" << first << "\"" << endl;
    }
}

static void findBaseAndOffset(Expression *expr, Expression *&base, int &offset)
{
    int field;
    base = expr;
    offset = 0;
    while(base->isField(base,field))
    {
        offset += field;
    }
}

/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand side is an integer
 *		scalar.
 */

void Assignment::generate()
{
    Expression *base;
    int offset;
    findBaseAndOffset(_left,base,offset);
    _right->generate();

    if(_right->_register == nullptr)
    {
        load(_right, getreg());
    }
    if(base->isDereference(base))
    {
        base->generate();
        if(base->_register == nullptr)
            load(base, getreg());
        if(_left->type().size() == 4)
        {
            cout << "\tmovl\t" << _right;
        }
        else if(_right->_register != nullptr)
        {
            cout << "\tmovb\t" << _right->_register->byte();
        }
        else
        {
            cout << "\tmovl\t" << _right;
        }

        cout << ",";
        if(offset != 0)
        {
            cout << offset;
        }
        cout << "(" << base << ")" << endl;
        assign(base, nullptr);
    }
    else
    {
        if(base->type().size() == 1)
        {
            cout << "\tmovb\t" << _right << ", " << offset << "+" << base << endl;
        }
        else
        {
            cout << "\tmovl\t" << _right << ", " << offset << "+" << base << endl;
        }
    }
    assign(_right, nullptr);
}

void assign(Expression *expr, Register *reg)
{
    if (expr != nullptr) 
    {
        if (expr->_register != nullptr)
        {
            expr->_register->_node = nullptr;
        }
        expr->_register = reg;
    }
    if (reg != nullptr) 
    {
        if (reg->_node != nullptr)
        {
            reg->_node->_register = nullptr;
        }
        reg->_node = expr;
    }
}

void load(Expression *expr, Register *reg)
{
    if (reg->_node != expr) 
    {
        if(reg->_node != nullptr)
        {
            unsigned n = reg->_node->type().size();
            offset -= n;
            reg->_node->_offset = offset;
            cout << (n == 1 ? "\tmovb\t" : "\tmovl\t");
            cout << reg << ", " << offset << "(%ebp)" << endl;
        }
        if (expr != nullptr) 
        {
            unsigned n = expr->type().size();
            cout << (n == 1 ? "\tmovb\t" : "\tmovl\t");
            cout << expr << ", " << reg->name(n) << endl;
        }
        assign(expr, reg);
    }
}

Register *getreg()
{
    for (auto reg : registers)
        if (reg->_node == nullptr)
            return reg;
    load(nullptr, registers[0]);
    return registers[0];
}

void compute(Expression *result, Expression *left, Expression *right, const string &op)
{
    left->generate();
    right->generate();
    if (left->_register == nullptr)
        load(left, getreg());
    cout << "\t" << op << "\t" << right << ", " << left << endl;
    assign(right, nullptr);
    assign(result, left->_register);
}

void Add::generate() 
{
    compute(this, _left, _right,"addl");
}

void Subtract::generate() 
{
    compute(this, _left, _right,"subl");
}

void Multiply::generate() 
{
    compute(this, _left, _right,"imull");
}

void Divide::generate()
{
    unsigned num;
    _left->generate();
    _right->generate();

    if(_left->_register == nullptr)
    {
        load(_left,getreg());
    }
    load(nullptr,edx);

    if(_right->isNumber(num))
    {
        load(_right,ecx);
    }

    cout << "\tcltd\t" << endl;
    cout << "\tidivl\t" << _right << endl;

    assign(_right,nullptr);
    assign(_left,nullptr);
    
    assign(this,eax);
}

void Remainder::generate()
{
    unsigned num;
    _left->generate();
    _right->generate();

    if(_left->_register == nullptr)
    {
        load(_left,getreg());
    }
    load(nullptr,edx);

    if(_right->isNumber(num))
    {
        load(_right,ecx);
    }

    cout << "\tcltd\t" << endl;
    cout << "\tidivl\t" << _right << endl;

    assign(_right,nullptr);
    assign(_left,nullptr);
    
    assign(this,edx);
}

void compare(Expression *result, Expression *left, Expression *right, const string &op)
{
    left->generate();
    right->generate();

    if(left->_register == nullptr)
    {
        load(left,getreg());
    }

    cout << "\tcmpl\t" << right << ", " << left << endl;
    cout << "\t" << op << "\t" << left->_register->byte() << endl;
    cout << "\tmovzbl\t" << left->_register->byte() << ", " << left->_register << endl;

    assign(result, left->_register);
}

void LessThan::generate() 
{
    compare(this, _left, _right, "setl");
}

void GreaterThan::generate() 
{
    compare(this, _left, _right, "setg");
}

void LessOrEqual::generate() 
{
    compare(this, _left, _right, "setle");
}

void GreaterOrEqual::generate() 
{
    compare(this, _left, _right, "setge");
}

void Equal::generate() 
{
    compare(this, _left, _right, "sete");
}

void NotEqual::generate() 
{
    compare(this, _left, _right, "setne");
}

void Cast::generate() 
{
    _expr->generate();
    if(_expr->_register == nullptr)
        load(_expr, getreg());
    if(_expr->type().size() == 1)
        cout << "\tmovsbl\t" << _expr->_register->byte() << "," << _expr << endl; 
    assign(this, _expr->_register);
}

void Not::generate()
{
    _expr->generate();
    if(_expr->_register == nullptr)
    {
        load(_expr,getreg());
    }
    cout << "\tcmpl\t" << "$0, " << _expr << endl;
    cout << "\tsete\t" << _expr->_register->byte() << endl;
    cout << "\tmovzbl\t" << _expr->_register->byte() << ", " << _expr << endl;
    assign(this, _expr->_register);
}

void Negate::generate() 
{
    _expr->generate();
    if(_expr->_register == nullptr)
    {
        load(_expr, getreg());
    }
    cout << "\tnegl\t" << _expr << endl;
    assign(this, _expr->_register);
}

void Dereference::generate()
{
    _expr->generate();
    if(_expr->_register == nullptr)
    {
        load(_expr,getreg());
    }
    if(_type.size() == 1)
    {
        cout << "\tmovb\t" << "(" << _expr << "), " << _expr << endl;
    }
    else
    {
        cout << "\tmovl\t" << "(" << _expr << "), " << _expr << endl;
    }
    assign(this, _expr->_register);
}

void Address::generate() 
{
    Expression *expr;
    if(_expr->isDereference(expr))
    {
        expr->generate();
        if(expr->_register == nullptr)
        {
            load(expr, getreg());
        }
        assign(this,expr->_register);
    }
    else
    {
        assign(this,getreg());
        cout << "\tleal\t" << _expr << ", " << this << endl;
    }
}

void Expression::test(const Label &label, bool ifTrue) 
{
    generate();
    if(_register == nullptr)
    {
        load(this, getreg());
    }
    cout << "\tcmpl\t$0, " << this << endl;
    cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;

    assign(this,nullptr);
}

void LogicalOr::generate()
{
    Label skip, shortCircuit;
    _left->test(skip,true);
    _right->test(skip,true);
    cout << "\tjmp\t" << shortCircuit << endl;
    assign(this,getreg());
    cout << skip << ":" << endl;
    cout << "\tmovl\t$1, " << this << endl;
    cout << shortCircuit << ":" << endl;
}

void LogicalAnd::generate()
{
    Label skip, shortCircuit;
    _left->test(skip,false);
    _right->test(skip,false);
    cout << "\tjmp\t" << shortCircuit << endl;
    assign(this,getreg());
    cout << skip << ":" << endl;
    cout << "\tmovl\t$0, " << this << endl;
    cout << shortCircuit << ":" << endl;
}

void Return::generate()
{
    _expr->generate();
    if(_expr->_register != eax)
    {
        load(_expr,eax);
    }
    cout << "\tjmp\t" << funcname << ".exit" << endl;
    assign(_expr,nullptr);
}

void If::generate()
{
    Label skip, exit;
    _expr->test(skip,false);
    _thenStmt->generate();
    if(_elseStmt != nullptr)
    {
        cout << "\tjmp\t" << exit << endl;
        cout << skip << ":" << endl;
        _elseStmt->generate();
        cout << exit << ":" << endl;
    }
    else
    {
        cout << skip << ":" << endl;
    }
}

void While::generate()
{
    Label loop, exit;
    cout << loop << ":" << endl;
    _expr->test(exit,false);
    _stmt->generate();
    cout << "\tjmp\t" << loop << endl;
    cout << exit << ":" << endl;
}

void For::generate()
{
    Label loop, exit;
    _init->generate();
    cout << loop << ":" << endl;
    _expr->test(exit, false);
    _stmt->generate();
    _incr->generate();
    cout << "\tjmp\t" << loop << endl;
    cout << exit << ":" << endl;
}

void Field::generate()
{
    int offset = 0;
    Expression *base,*expr;
    findBaseAndOffset(this,base,offset);
    if(base->isDereference(expr))
    {
        expr->generate();
        if(expr->_register == nullptr)
        {
            load(expr,getreg());
        }
        if(_type.size() == 1)
        {
            cout << "\tmovb\t";
        }
        else
        {
            cout << "\tmovl\t";
        }
        if(offset != 0)
        {
            cout << offset;
        }
        cout << " (" << expr << "), ";
        assign(this,expr->_register);
        cout << this << endl;
    }
    else
    {
        assign(this,getreg());
        if(_type.size() == 1)
        {
            cout << "\tmovb\t";
        }
        else
        {
            cout << "\tmovl\t";
        }
        cout << offset << "+" << base << ", " << this << endl;
    }
}

void String::operand(ostream &ostr) const 
{
    Label str;
    if(strings.count(_value) == 0)
    {
        strings.insert(std::pair<string, Label> (_value, str));
    }
    else
    {
        str = strings.find(_value)->second;
    }
    ostr << str;
}