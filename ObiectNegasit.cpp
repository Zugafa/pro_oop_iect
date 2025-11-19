//
// Created by nitui on 11/19/2025.
//

#include "ObiectNegasit.h"

ObiectNegasit::ObiectNegasit(const std::string& eticheta) : ScriptException(
                                                                "Obiectul '" + eticheta + "' nu a fost gasit."),
                                                            etichetaCautata(eticheta)
{
}

std::string ObiectNegasit::getSugestie() const
{
    return "Verifica daca ai scris corect eticheta '" + etichetaCautata +
        "' (case-sensitive) sau listeaza tot continutul seifului inainte de stergere.";
}