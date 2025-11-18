#ifndef OOP_SCRIPTEXCEPTION_H
#define OOP_SCRIPTEXCEPTION_H
#include <exception>
#include <string>

class ScriptException : public std::exception
{
    std::string mesaj;

public:
    explicit ScriptException(const std::string& m) : mesaj{m}
    {
    };
    [[nodiscard]] const char* what() const noexcept override { return mesaj.c_str(); }
};


#endif //OOP_SCRIPTEXCEPTION_H