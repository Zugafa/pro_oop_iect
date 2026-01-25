//
// Created by nitui on 11/19/2025.
//

#include "EroareAcces.h"

EroareAcces::EroareAcces(int index, int max) : ScriptException("Eroare de acces la memorie (Index Out of Bounds)."),
                                               indexCerut(index), // Îl inițializăm
                                               dimensiuneMaxima(max)
{
}

std::string EroareAcces::getSugestie() const
{
    std::string explicatie = "Ai incercat sa accesezi indexul " + std::to_string(indexCerut);

    if (indexCerut < 0)
    {
        explicatie += ", dar indecsii nu pot fi negativi.";
    }
    else
    {
        explicatie += ", dar seiful are doar " + std::to_string(dimensiuneMaxima) + " elemente.";
    }

    // Sugestia finală
    if (dimensiuneMaxima > 0)
    {
        explicatie += " Incearca un index intre 0 si " + std::to_string(dimensiuneMaxima - 1) + ".";
    }
    else
    {
        explicatie += " Seiful este gol!";
    }

    return explicatie;
}