//
// Created by nitui on 11/2/2025.
//

#include "DateAutentificare.h"
#include "EroareValidare.h"

// IMPLEMENTARE CONSTRUCTOR (4 Argumente)
DateAutentificare::DateAutentificare(const std::string& nPlat,
                                     const std::string& nUtiliz,
                                     const std::string& pass)
    : Seif{nPlat},
      numePlatforma{nPlat},
      numeUtilizator{nUtiliz},
      parola{pass}
{
}

void DateAutentificare::setEticheta(const std::string& eticheta) {
    Seif::setEticheta(eticheta);
    this->numePlatforma = eticheta;
}

void DateAutentificare::setter_parola(const std::string& nouaParola)
{
    if (nouaParola.length() < Configuratie::getInstance().getLungimeMinimaParola())
    {
        throw EroareValidare(
            numePlatforma,
            static_cast<int>(nouaParola.length()),
            static_cast<int>(Configuratie::getInstance().getLungimeMinimaParola())
        );
    }

    bool areLiteraMare = false;
    bool areCaracterSpecial = false;
    std::string caractereValide = Configuratie::getInstance().getCaractereSpecialeValide();

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
        return;
    }
    throw EroareValidare(numePlatforma, "Lipsesc caractere obligatorii (Majuscula/Simbol).");
}

void DateAutentificare::setter_numeUtilizator(const std::string& nouNume)
{
    this->numeUtilizator = nouNume;
}

void DateAutentificare::CriptareVigenere()
{
    std::string Cheie = Configuratie::getInstance().getCheieVigenere();
    int pozCheie = 0;
    const int lenCheie = static_cast<int>(Cheie.length());
    for (auto& literaParola : parola)
    {
        if (!std::isalpha(literaParola)) continue;
        char baza = std::isupper(literaParola) ? 'A' : 'a';
        int valLitera = literaParola - baza;
        int valCheie = Cheie[pozCheie % lenCheie] - 'A';
        int valNouaLitera = (valLitera + valCheie) % 26;
        literaParola = static_cast<char>(valNouaLitera + baza);
        pozCheie++;
    }
}

void DateAutentificare::deCriptareVigenere()
{
    std::string Cheie = Configuratie::getInstance().getCheieVigenere();
    int pozCheie = 0, lenCheie = static_cast<int>(Cheie.length());
    for (auto& literaParola : parola)
    {
        if (!std::isalpha(literaParola)) continue;
        char baza = std::isupper(literaParola) ? 'A' : 'a';
        int valLiteraCriptata = literaParola - baza;
        int valCheie = Cheie[pozCheie % lenCheie] - 'A';
        int valLiteraNoua = (valLiteraCriptata - valCheie + 26) % 26;
        literaParola = static_cast<char>(valLiteraNoua + baza);
        pozCheie++;
    }
}

void DateAutentificare::verificaSecuritate() const
{
    if (parola.length() < 12)
        std::cout << "[WARN] Contul " << getEticheta() << " are securitate medie.\n";
    else
        std::cout << "[OK] Contul " << getEticheta() << " este securizat.\n";
}

std::map<std::string, std::string> DateAutentificare::getDatePentruSalvare() const
{
    std::map<std::string, std::string> date;
    date["platforma"] = getEticheta(); // Folosim eticheta ca platforma
    date["utilizator"] = numeUtilizator;
    date["parola"] = parola;
    return date;
}
