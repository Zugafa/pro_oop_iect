#ifndef OOP_OBIECTNEGASIT_H
#define OOP_OBIECTNEGASIT_H
#include "ScriptException.h"
#include <string>

/**
 * @class ObiectNegasit
 * @brief Exception thrown when a requested object is not found in the vault.
 */
class ObiectNegasit : public ScriptException
{
private:
    std::string etichetaCautata; ///< The label that was searched for

public:
    /**
     * @brief Constructor for ObiectNegasit.
     * @param eticheta The label that could not be found.
     */
    explicit ObiectNegasit(const std::string& eticheta);

    /**
     * @brief Provides a suggestion for the user when an object is missing.
     * @return Suggestion string.
     */
    [[nodiscard]] std::string getSugestie() const override;
};
#endif