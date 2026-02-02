#include "Configuratie.h"
#include <iostream>

/**
 * @brief Default constructor initializing settings.
 */
Configuratie::Configuratie() : cheieVigenere{"ENCRYPTER"},
                               caractereSpecialeValide{"!@#$%^&*"},
                               lungimeMinimaParola{8u}
{
}

/**
 * @brief Returns the singleton instance.
 */
Configuratie& Configuratie::getInstance()
{
    static Configuratie instance;
    return instance;
}

/**
 * @brief Formats and outputs configuration details.
 */
std::ostream& operator<<(std::ostream& out, const Configuratie& config)
{
    out << "Configuration Settings:\n";
    out << "  -> Crypto Key: " << config.cheieVigenere << "\n";
    out << "  -> Min Password Length: " << config.lungimeMinimaParola << "\n";
    out << "  -> Special Characters: " << config.caractereSpecialeValide << "\n";
    return out;
}
