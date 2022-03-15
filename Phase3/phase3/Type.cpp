#include <cassert>
#include "Type.h"

Type::Type(int kind, const string &specifier, unsigned indirection) : _kind(kind), _specifier(specifier), _indirection(indirection) 
{
    _length = 0;
    _parameters = nullptr;
}

Type::Type() : _kind(ERROR), _specifier("error"), _indirection(0)
{
    _length = 0;
    _parameters = nullptr;
}

bool Type::operator !=(const Type &rhs) const 
{
    return !operator==(rhs);
}

bool Type::operator ==(const Type &rhs) const
{
    if(_kind != rhs._kind)
    {
        return false;
    }
    if(_kind == ERROR)
    {
        return true;
    }
    if(_specifier != rhs._specifier)
    {
        return false;
    }
    if(_indirection != rhs._indirection)
    {
        return false;
    }
    if(_kind == SCALAR)
    {
        return true;
    }
    if(_kind == ARRAY)
    {
        return _length == rhs._length;
    }
    if(!_parameters || !rhs._parameters)
    {
        return true;
    }
    return *_parameters == *rhs._parameters;
}

bool Type::isStruct() const
{
    return _kind != ERROR && _specifier != "char" && _specifier != "int";
}

std::ostream &operator <<(std::ostream &ostr, const Type &type)
{
    ostr << "(" << type.specifier() << "," << type.indirection() << ",";

    if(type.isArray())
    {
        ostr << "ARRAY";
    }
    else if(type.isError())
    {
        ostr << "ERROR";
    }
    else if(type.isCallBack())
    {
        ostr << "CALLBACK";
    }
    else if(type.isFunction())
    {
        ostr << "FUNCTION";
    }
    else
    {
        assert(type.isScalar());
        ostr << "SCALAR";
    }

    ostr << ")";
    return ostr;
}