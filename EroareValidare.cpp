#include "EroareValidare.h"

// Constructor 1 (Lungime)
EroareValidare::EroareValidare(const std::string& cont, int len, int min)
    : ScriptException("Eroare Validare Lungime"), // <--- TEXT STATIC SCURT
      numeCont(cont),
      lungimePrimita(len),
      lungimeMinima(min),
      esteEroareContinut(false)
{
}

// Constructor 2 (Continut)
EroareValidare::EroareValidare(const std::string& cont)
    : ScriptException("Eroare Validare Continut"), // <--- TEXT STATIC SCURT
      numeCont(cont),
      lungimePrimita(0),
      lungimeMinima(0),
      esteEroareContinut(true)
{
}

std::string EroareValidare::getSugestie() const
{
    if (esteEroareContinut)
        return "La contul '" + numeCont +
            "': Asigura-te ca parola contine cel putin o litera mare si un caracter special.";

    else
    {
        int diferenta = lungimeMinima - lungimePrimita;
        return "La contul '" + numeCont + "': Parola este prea scurta (" +
            std::to_string(lungimePrimita) + " vs " + std::to_string(lungimeMinima) +
            "). Mai adauga " + std::to_string(diferenta) + " caractere.";
    }
}
