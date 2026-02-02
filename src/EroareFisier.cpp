#include "EroareFisier.h"

/**
 * @brief Constructor initializing the exception with the generic message and specific filename.
 * @param nume The path to the missing resource.
 */
EroareFisier::EroareFisier(const std::string& nume)
    : ScriptException("Resource Loading Error"), numeFisier(nume)
{
}

/**
 * @brief Returns a user-friendly suggestion to locate the missing file.
 * @return Formatted string guiding the user to the 'images/' or 'fonts/' directory.
 */
std::string EroareFisier::getSugestie() const
{
    return "File '" + numeFisier + "' is missing or corrupt.\n"
        "Please ensure it exists in the 'images/' or 'fonts/' folder next to the executable.";
}