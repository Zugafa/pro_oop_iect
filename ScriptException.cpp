#include "ScriptException.h"

const char* ScriptException::what() const noexcept
{
    return mesajEroare.c_str();
}