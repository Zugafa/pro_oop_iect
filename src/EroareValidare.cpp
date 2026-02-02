#include "EroareValidare.h"

/**
 * @brief Constructor for length errors.
 */
EroareValidare::EroareValidare(const std::string& cont, int len, int min) :
    ScriptException("Validation Error (Length)"),
    lungimePrimita(len),
    lungimeMinima(min),
    esteEroareContinut(false),
    cont(cont)
{
}

/**
 * @brief Constructor for content errors.
 */
EroareValidare::EroareValidare(const std::string& cont, const std::string& motiv) : ScriptException(
        "Validation Error (Content)"),
    lungimePrimita(0),
    lungimeMinima(0),
    esteEroareContinut(true),
    cont(cont),
    motiv(motiv)
{
}

/**
 * @brief Returns a suggestion based on the type of validation error.
 */
std::string EroareValidare::getSugestie() const
{
    if (esteEroareContinut)
    {
        return "For account '" + cont + "', the problem is: " + motiv;
    }
    else
    {
        int diferenta = lungimeMinima - lungimePrimita;
        return "For account '" + cont + "', the password is too short. Add at least " +
            std::to_string(diferenta) + " more characters.";
    }
}