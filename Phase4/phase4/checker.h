/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include <string>
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

void openStruct(const std::string &name);
void closeStruct(const std::string &name);

void declareSymbol(const std::string &name, const Type &type, bool = false);

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

Type checkReturn(const Type &left, const Type &enclosing);
Type checkWhileForIf(const Type &left);
Type checkAssignment(const Type &left, const Type &right, bool &lvalue);
Type checkLogical(const Type &left, const Type &right, const std::string &op);
Type checkEquality(const Type &left, const Type &right, const std::string &op);
Type checkRelational(const Type &left, const Type &right, const std::string &op);
Type checkAdditive(const Type &left, const Type &right, const std::string &op);
Type checkMultiplicative(const Type &left, const Type &right, const std::string &op);
Type checkNegate(const Type &right);
Type checkNot(const Type &right);
Type checkAddress(const Type &right, const bool lvalue);
Type checkDereference(const Type &right);
Type checkSizeof(const Type &right);
Type checkTypecast(const Type &right, const std::string specifier, const unsigned indirection);
Type checkArrayRef(const Type &left, const Type &ex);
Type checkFunctionCall(const Type &left, const Parameters &p);
Type checkDirectRef(const Type &left, const std::string id);
Type checkIndirectRef(const Type &left, const std::string id);



# endif /* CHECKER_H */
