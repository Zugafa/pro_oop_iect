//
// Created by nitui on 11/2/2025.
//

#include "Configuratie.h"
#include <iostream>

Configuratie::Configuratie() : cheieVigenere{"ENCRYPTER"},
                               caractereSpecialeValide{"!@#$%^&*"},
                               lungimeMinimaParola{8u}
{
}

std::ostream& operator<<(std::ostream& out, const Configuratie& config)
{
    out << "Setari Configurare:\n";
    out << "  -> Cheie Vigenere: " << config.cheieVigenere << "\n";
    out << "  -> Lungime Minima Parola: " << config.lungimeMinimaParola << "\n";
    out << "  -> Caractere Speciale: " << config.caractereSpecialeValide << "\n";
    return out;
}