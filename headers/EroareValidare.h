#ifndef OOP_EROAREVALIDARE_H
#define OOP_EROAREVALIDARE_H
#include "ScriptException.h"
#include <string>

/**
 * @class EroareValidare
 * @brief Exception thrown during validation of vault items (e.g., password length or content).
 */
class EroareValidare : public ScriptException
{
private:
    int lungimePrimita;      ///< The length that was provided
    int lungimeMinima;      ///< The minimum required length
    bool esteEroareContinut; ///< Flag indicating if it's a content-related error (not length)
    std::string cont;        ///< Account/item identifier
    std::string motiv;       ///< Reason for content error

public:
    /**
     * @brief Constructor for length-related validation errors.
     * @param cont Account name.
     * @param len Provided length.
     * @param min Required minimum length.
     */
    EroareValidare(const std::string& cont, int len, int min);

    /**
     * @brief Constructor for content-related validation errors.
     * @param cont Account name.
     * @param motiv Reason for the failure.
     */
    EroareValidare(const std::string& cont, const std::string& motiv);

    /**
     * @brief Provides a detailed suggestion on how to fix the validation error.
     * @return Suggestion string.
     */
    [[nodiscard]] std::string getSugestie() const override;
};
#endif