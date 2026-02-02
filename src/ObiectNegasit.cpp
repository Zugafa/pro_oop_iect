//
// Created by nitui on 11/19/2025.
//

#include "ObiectNegasit.h"

/**
 * @brief Constructor for ObiectNegasit.
 */
ObiectNegasit::ObiectNegasit(const std::string& eticheta) : ScriptException(
                                                                "Object '" + eticheta + "' was not found."),
                                                            etichetaCautata(eticheta)
{
}

/**
 * @brief Returns a suggestion for finding the missing object.
 */
std::string ObiectNegasit::getSugestie() const
{
    return "Check if you typed the label '" + etichetaCautata +
        "' correctly (case-sensitive) or list all vault contents before trying again.";
}