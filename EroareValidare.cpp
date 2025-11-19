#include "EroareValidare.h"

// Modificăm constructorul pentru a fi mai blând cu stiva
EroareValidare::EroareValidare(const std::string& cont, int len, int min)
    : ScriptException("Eroare validare lungime") // Mesaj scurt inițial
{
    // Construim mesajul detaliat AICI, nu în lista de inițializare
    mesajEroare = "Validare esuata pentru contul '" + cont + "' (Lungime " +
        std::to_string(len) + " vs " + std::to_string(min) + ").";

    lungimePrimita = len;
    lungimeMinima = min;
    esteEroareContinut = false;
}

EroareValidare::EroareValidare(const std::string& cont, const std::string& motiv)
    : ScriptException("Eroare validare continut") // Mesaj scurt inițial
{
    mesajEroare = "Validare esuata pentru contul '" + cont + "': " + motiv;

    lungimePrimita = 0;
    lungimeMinima = 0;
    esteEroareContinut = true;
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
