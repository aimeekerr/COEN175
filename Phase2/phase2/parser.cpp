# include <set>
# include <cstdio>
# include <cerrno>
# include <cctype>
# include <cstdlib>
# include <iostream>
# include "string.h"
# include "lexer.h"
# include "tokens.h"

using namespace std;

string lexbuf;
int lookahead;

void match(int token);
void rem_decl();
void glob_decl();
void pointers();
int specifier();
void parameters();
void parameter_list();
void parameter();
void declarations();
void declaration();
void declarator_list();
void declarator();
void statements();
void statement();
void assignment();
void expression_list();
void primary_exp(bool check);
void postfix_exp(bool check);
void prefix_exp();
void multiplicative_exp();
void additive_exp();
void relational_exp();
void equal_exp();
void and_exp();
void expression();

void fun_or_glo()
{
  if(specifier() == STRUCT && lookahead == '{')
  {
    match('{');
    declaration();
    declarations();
    match('}');
    match(';');
  }
  else
  {
    pointers();
    if(lookahead == '(')
    {
      match('(');
      match('*');
      match(ID);
      match(')');
      match('(');
      match(')');
      rem_decl();
    }
    else
    {
      match(ID);
      if(lookahead == '[')
      {
        match('[');
        match(NUM);
        match(']');
        rem_decl();
      }
      else if(lookahead == '(')
      {
        match('(');
        if(lookahead == ')')
        {
          match(')');
          rem_decl();
        }
        else
        {
          parameters();
          match(')');
          match('{');
          declarations();
          statements();
          match('}');
        }
      }
      else
      {
        rem_decl();
      }
    }
  }
}

void glob_decl()
{
  //cout << "glob_decl" << endl;
  pointers();
  if(lookahead == '(')
  {
    match('(');
    match('*');
    match(ID);
    match(')');
    match('(');
    match(')');
  }
  else
  {
    match(ID);
    if(lookahead == '[')
    {
      match('[');
      match(NUM);
      match(']');
    }
    else if(lookahead == '(')
    {
      match('(');
      match(')');
    }
  }
}

void rem_decl()
{
  //cout << "rem_decl" << endl;
  while(lookahead == ',')
  {
    match(',');
    glob_decl();
  }
  match(';');
}

void pointers()
{
  //cout << "pointers" << endl;
  while(lookahead == '*')
  {
    match('*');
  }
}

int specifier()
{
  //cout << "specifier" << endl;
  if(lookahead == INT)
  {
    match(INT);
    return INT;
  }
  else if(lookahead == CHAR)
  {
    match(CHAR);
    return CHAR;
  }
  match(STRUCT);
  match(ID);
  return STRUCT;
}

void parameters()
{
  //cout << "parameters" << endl;
  if(lookahead == VOID)
  {
    match(VOID);
  }
  else
  {
    parameter_list();
  }
}

void parameter_list()
{
  //cout << "parameter_list" << endl;
  parameter();
  while(lookahead == ',')
  {
    match(',');
    parameter();
  }
}

void parameter()
{
  //cout << "parameter" << endl;
  specifier();
  pointers();
  if(lookahead == ID)
  {
    match(ID);
  }
  else
  {
    match('(');
    match('*');
    match(ID);
    match(')');
    match('(');
    match(')');
  }
}

void declarations()
{
  //cout << "declarations" << endl;
  while(lookahead == INT || lookahead == CHAR || lookahead == STRUCT)
  {
    declaration();
  }
}

void declaration()
{
  //cout << "declaration" << endl;
  specifier();
  declarator_list();
  match(';');
}

void declarator_list()
{
  //cout << "declarator_list" << endl;
  declarator();
  while(lookahead == ',')
  {
    match(',');
    declarator();
  }
}

void declarator()
{
  //cout << "declarator" << endl;
  pointers();
  if(lookahead == ID)
  {
    match(ID);
    if(lookahead == '[')
    {
      match('[');
      match(NUM);
      match(']');
    }
  }
    else if(lookahead == '(')
    {
      match('(');
      match('*');
      match(ID);
      match(')');
      match('(');
      match(')');
    }
}

void statements()
{
  //cout << "statements" << endl;
  while(lookahead != '}')
  {
    statement();
  }
}

void statement()
{
  //cout << "statement" << endl;
  if(lookahead == '{')
  {
    match('{');
    declarations();
    statements();
    match('}');
  }
  else if(lookahead == RETURN)
  {
    match(RETURN);
    expression();
    match(';');
  }
  else if(lookahead == WHILE)
  {
    match(WHILE);
    match('(');
    expression();
    match(')');
    statement();
  }
  else if(lookahead == FOR)
  {
    match(FOR);
    match('(');
    assignment();
    match(';');
    expression();
    match(';');
    assignment();
    match(')');
    statement();
  }
  else if(lookahead == IF)
  {
    match(IF);
    match('(');
    expression();
    match(')');
    statement();
    if(lookahead == ELSE)
    {
      match(ELSE);
      statement();
    }
  }
  else
  {
    assignment();
    if(lookahead == ';')
    {
      match(';');
    }
  }
}

void assignment()
{
  //cout << "assignment" << endl;
  expression();
  if(lookahead == '=')
  {
    match('=');
    expression();
  }
}

void expression_list()
{
  //cout << "expression_list" << endl;
  expression();
  while(lookahead == ',')
  {
    match(',');
    expression();
  }
}

void primary_exp(bool check)
{
  //cout << "primary_exp" << endl;
  if(check == true)
  {
    expression();
    match(')');
  }
  else if(lookahead == ID)
  {
    match(ID);
  }
  else if(lookahead == NUM)
  {
    match(NUM);
  }
  else if(lookahead == STRING)
  {
    match(STRING);
  }
  else if(lookahead == CHARACTER)
  {
    match(CHARACTER);
  }
}   

void postfix_exp(bool check)
{
  //cout << "postfix_exp" << endl;
  primary_exp(check);
  while(lookahead == '[' || lookahead == '(' || lookahead == '.' || lookahead == ARROW)
  {
    if(lookahead == '[')
    {
      match('[');
      expression();
      match(']');
      cout << "index" << endl;
    }
    else if(lookahead == '.')
    {
      match('.');
      match(ID);
      cout << "dot" << endl;
    }
    else if(lookahead == ARROW)
    {
      match(ARROW);
      match(ID);
      cout << "arrow" << endl;
    }
    else if(lookahead == '(')
    {
      match('(');
      if(lookahead == ')')
      {
        match(')');
      }
      else
      {
        expression_list();
        match(')');
      }
      cout << "call" << endl;
    }
  }
}   

void prefix_exp()
{
  //cout << "prefix_exp" << endl;
  if(lookahead == '!')
  {
    match('!');
    prefix_exp();
    cout << "not" << endl;
  }
  else if(lookahead == '-')
  {
    match('-');
    prefix_exp();
    cout << "neg" << endl;
  }
  else if(lookahead == '&')
  {
    match('&');
    prefix_exp();
    cout << "addr" << endl;
  }
  else if(lookahead == '*')
  {
    match('*');
    prefix_exp();
    cout << "deref" << endl;
  }
  else if(lookahead == SIZEOF)
  {
    match(SIZEOF);
    if(lookahead == '(')
    {
      match('(');
      if(lookahead == CHAR || lookahead == INT || lookahead == STRUCT)
      {
        specifier();
        pointers();
        match(')');
      }
      else
      {
        postfix_exp(true);
      }
    }
    else
    {
      prefix_exp();
    }
    cout << "sizeof" << endl;
  }
  else if(lookahead == '(')
  {
    match('(');
    if(lookahead == CHAR || lookahead == INT || lookahead == STRUCT)
    {
      specifier();
      pointers();
      match(')');
      prefix_exp();
      cout << "cast" << endl;
    }
    else
    {
      postfix_exp(true);
    }
  }
  else
  {
    postfix_exp(false);
  }
}

void multiplicative_exp()
{
  //cout << "multiplicative_exp" << endl;
  prefix_exp();
  while(lookahead == '*' || lookahead == '/' || lookahead == '%')
  {
    int lookbehind = lookahead;
    match(lookahead);
    prefix_exp();
    if(lookbehind == '*')
    {
      cout << "mul" << endl;
    }
    else if(lookbehind == '/')
    {
      cout << "div" << endl;
    }
    else if(lookbehind == '%')
    {
      cout << "rem" << endl;
    }
  }
}

void additive_exp()
{
  //cout << "additive_exp" << endl;
  multiplicative_exp();
  while(lookahead == '+' || lookahead == '-')
  {
    int lookbehind = lookahead;
    match(lookahead);
    multiplicative_exp();
    if(lookbehind == '+')
    {
      cout << "add" << endl;
    }
    else if(lookbehind == '-')
    {
      cout << "sub" << endl;
    }
  }
}

void relational_exp()
{
  //cout << "relational_exp" << endl;
  additive_exp();
  while(lookahead == '<' || lookahead == '>' || lookahead == LEQ || lookahead == GEQ)
  {
    int lookbehind = lookahead;
    match(lookahead);
    additive_exp();
    if(lookbehind == '<')
    {
      cout << "ltn" << endl;
    }
    else if(lookbehind == '>')
    {
      cout << "gtn" << endl;
    }
    else if(lookbehind == LEQ)
    {
      cout << "leq" << endl;
    }
    else
    {
      cout << "geq" << endl;
    }
  }
}

void equal_exp()
{
  //cout << "equal_exp" << endl;
  relational_exp();
  while(lookahead == EQL || lookahead == NEQ)
  {
    int lookbehind = lookahead;
    match(lookahead);
    relational_exp();
    if(lookbehind == EQL)
    {
      cout << "eql" << endl;
    }
    else if(lookbehind == NEQ)
    {
      cout << "neq" << endl;
    }
  }
}

void and_exp()
{
  //cout << "and_exp" << endl;
  equal_exp();
  while(lookahead == AND)
  {
    match(AND);
    equal_exp();
    cout << "and" << endl;
  }
}

void expression()
{
  //cout << "expression" << endl;
  and_exp();
  while(lookahead == OR)
  {
    match(OR);
    and_exp();
    cout << "or" << endl;
  }
}   

void match(int token)
{
  //cout << lookahead << " and " << token << endl;
  if(lookahead == token)
  {
    lookahead = lexan(lexbuf);
  }
  else
  {
    cout << "error at: " << lexbuf << endl;
    exit(1);
  }
}

int main()
{
  lookahead = lexan(lexbuf);

  while(lookahead != DONE)
  {
    fun_or_glo();
  }
}
