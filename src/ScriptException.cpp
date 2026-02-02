#include "ScriptException.h"

/**
 * @brief Implementation of what() to return the error message.
 */
const char* ScriptException::what() const noexcept
{
    return mesajEroare.c_str();
}