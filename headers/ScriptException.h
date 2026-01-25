#ifndef OOP_SCRIPTEXCEPTION_H
#define OOP_SCRIPTEXCEPTION_H
#include <exception>
#include <string>

class ScriptException : public std::exception
{
protected:
    std::string mesajEroare;

public:
    explicit ScriptException(const std::string& m) : mesajEroare(m)
    {
    }

    [[nodiscard]] const char* what() const noexcept override;

    [[nodiscard]] virtual std::string getSugestie() const = 0;

    ~ScriptException() override = default;
};
#endif