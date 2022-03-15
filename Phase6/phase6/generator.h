/*
 * File:	generator.h
 *
 * Description:	This file contains the function declarations for the code
 *		generator for Simple C.  Most of the function declarations
 *		are actually member functions provided as part of Tree.h.
 */

# ifndef GENERATOR_H
# define GENERATOR_H
# include <iostream>
# include "Scope.h"
# include "Label.h"
# include "string.h"
# include "Tree.h"
# include "machine.h"
# include "Register.h"

using namespace std;

void generateGlobals(Scope *scope);
void assign(Expression *expr, Register *reg);
void load(Expression *expr, Register *reg);
Register *getreg();
void compute(const string &op);

# endif /* GENERATOR_H */
