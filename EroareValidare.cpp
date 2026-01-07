#include "EroareValidare.h"

EroareValidare::EroareValidare(const std::string& cont, int len, int min) :
    ScriptException("Eroare validare (Lungime)"),
    lungimePrimita(len),
    lungimeMinima(min),
    esteEroareContinut(false),
    cont(cont)
{
}

EroareValidare::EroareValidare(const std::string& cont, const std::string& motiv) : ScriptException(
        "Eroare validare (Conținut)"),
    lungimePrimita(0),
    lungimeMinima(0),
    esteEroareContinut(true),
    cont(cont),
    motiv(motiv)
{
}

std::string EroareValidare::getSugestie() const
{
    if (esteEroareContinut)
    {
        return "Pentru contul '" + cont + "', problema este: " + motiv;
    }
    else
    {
        int diferenta = lungimeMinima - lungimePrimita;
        return "Pentru contul '" + cont + "', parola este prea scurta. Mai adauga cel putin " +
            std::to_string(diferenta) + " caractere.";
    }
}