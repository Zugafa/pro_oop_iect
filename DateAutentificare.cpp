//
// Created by nitui on 11/2/2025.
//

#include "DateAutentificare.h"

void DateAutentificare::setter_parola(const std::string& nouaParola)
{
    // Folosim obiectul 'config'
    if (nouaParola.length() < config.getLungimeMinimaParola())
    {
        std::cout << "[INFO] Parola n-a fost setata (prea scurta).\n";
        return;
    }

    bool areLiteraMare = false;
    bool areCaracterSpecial = false;
    std::string caractereValide = config.getCaractereSpecialeValide();

    for (const auto& litera : nouaParola)
    {
        if (isupper(litera)) areLiteraMare = true;
        else if (caractereValide.find(litera) != std::string::npos) areCaracterSpecial = true;
        if (areLiteraMare && areCaracterSpecial)
            break;
    }

    if (areLiteraMare && areCaracterSpecial)
    {
        this->parola = nouaParola;
        std::cout << "[INFO] Parola a fost setata cu succes pentru " << this->numePlatforma << "!\n";
        return;
    }
    std::cout << "Parola trebuie sa contina o litera mare si un caracter special\n";
}

void DateAutentificare::CriptareVigenere()
{
    std::string Cheie = config.getCheieVigenere();
    int pozCheie = 0, lenCheie = Cheie.length();
    for (auto& literaParola : parola)
    {
        if (!std::isalpha(literaParola)) continue;
        char baza = std::isupper(literaParola) ? 'A' : 'a';
        int valLitera = literaParola - baza;
        int valCheie = Cheie[pozCheie % lenCheie] - 'A';
        int valNouaLitera = (valLitera + valCheie) % 26;
        literaParola = valNouaLitera + baza;
        pozCheie++;
    }
}

void DateAutentificare::deCriptareVigenere()
{
    std::string Cheie = config.getCheieVigenere();
    int pozCheie = 0, lenCheie = Cheie.length();
    for (auto& literaParola : parola)
    {
        if (!std::isalpha(literaParola)) continue;
        char baza = std::isupper(literaParola) ? 'A' : 'a';
        int valLiteraCriptata = literaParola - baza;
        int valCheie = Cheie[pozCheie % lenCheie] - 'A';
        int valLiteraNoua = (valLiteraCriptata - valCheie + 26) % 26;
        literaParola = valLiteraNoua + baza;
        pozCheie++;
    }
}