#ifndef OOP_SCRIPTEXCEPTION_H
#define OOP_SCRIPTEXCEPTION_H
#include <exception>
#include <string>

/**
 * @class ScriptException
 * @brief Base class for custom exceptions in the S.C.R.I.P.T project.
 * 
 * Inherits from std::exception and provides a virtual method for suggestions.
 */
class ScriptException : public std::exception
{
protected:
    std::string mesajEroare;    ///< Error message

public:
    /**
     * @brief Constructor for ScriptException.
     * @param m The error message.
     */
    explicit ScriptException(const std::string& m) : mesajEroare(m)
    {
    }

    /**
     * @brief Returns the error message.
     * @return C-style string containing the error message.
     */
    [[nodiscard]] const char* what() const noexcept override;

    /**
     * @brief Pure virtual method to get a suggestion for fixing the error.
     * @return A string containing the suggestion.
     */
    [[nodiscard]] virtual std::string getSugestie() const = 0;

    ~ScriptException() override = default;
};
#endif