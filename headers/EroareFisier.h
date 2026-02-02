#ifndef OOP_EROAREFISIER_H
#define OOP_EROAREFISIER_H

#include "ScriptException.h"
#include <string>

/**
 * @class EroareFisier
 * @brief Exception thrown when a critical resource file (image, font) is missing or corrupt.
 * * Inherits from ScriptException and provides specific suggestions regarding file placement.
 */
class EroareFisier : public ScriptException {
private:
    std::string numeFisier; ///< The path or name of the missing file.

public:
    /**
     * @brief Constructor for EroareFisier.
     * @param nume The name/path of the file that could not be loaded.
     */
    explicit EroareFisier(const std::string& nume);

    /**
     * @brief Provides a suggestion to check specific resource folders.
     * @return A string containing instructions for the user (e.g., check 'images/' folder).
     */
    [[nodiscard]] std::string getSugestie() const override;
};

#endif //OOP_EROAREFISIER_H