#include "EroareAcces.h"

/**
 * @brief Constructor for EroareAcces.
 */
EroareAcces::EroareAcces(int index, int max) : ScriptException("Memory Access Error (Index Out of Bounds)."),
                                               indexCerut(index), 
                                               dimensiuneMaxima(max)
{
}

/**
 * @brief Returns an explanation and suggestion for the access error.
 */
std::string EroareAcces::getSugestie() const
{
    std::string explicatie = "You tried to access index " + std::to_string(indexCerut);

    if (indexCerut < 0)
        explicatie += ", but indices cannot be negative.";
    else
        explicatie += ", but the vault only has " + std::to_string(dimensiuneMaxima) + " elements.";

    if (dimensiuneMaxima > 0)
        explicatie += " Try an index between 0 and " + std::to_string(dimensiuneMaxima - 1) + ".";
    else
        explicatie += " The vault is empty!";

    return explicatie;
}