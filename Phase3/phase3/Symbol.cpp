#include "Symbol.h"

using namespace std;

Symbol::Symbol(string name, const Type& type, bool defined) : _name(name), _type(type), _defined(defined)
{
}