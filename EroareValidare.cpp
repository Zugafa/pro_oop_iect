//
// Created by nitui on 11/19/2025.
//

#include "EroareValidare.h"

EroareValidare::EroareValidare(const std::string& cont, int len, int min) : ScriptException(
                                                                                "Validare esuata pentru contul '" + cont
                                                                                + "' (Lungime)."),
                                                                            lungimePrimita(len),
                                                                            lungimeMinima(min),
                                                                            esteEroareContinut(false)
{
}

EroareValidare::EroareValidare(const std::string& cont, const std::string& motiv) : ScriptException(
        "Validare esuata pentru contul '" + cont + "': " + motiv),
    lungimePrimita(0),
    lungimeMinima(0),
    esteEroareContinut(true)
{
}

std::string EroareValidare::getSugestie() const
{
    if (esteEroareContinut)
    {
        return "Asigura-te ca parola contine cel putin o litera mare (A-Z) si un caracter special (!@#...).";
    }
    else
    {
        int diferenta = lungimeMinima - lungimePrimita;
        return "Parola este prea scurta. Mai adauga cel putin " +
            std::to_string(diferenta) + " caractere.";
    }
}