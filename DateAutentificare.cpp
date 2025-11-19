//
// Created by nitui on 11/2/2025.
//

#include "DateAutentificare.h"
#include "EroareValidare.h"

// IMPLEMENTARE CONSTRUCTOR (4 Argumente)
DateAutentificare::DateAutentificare(const std::string& nPlat,
                                     const std::string& nUtiliz,
                                     const std::string& pass,
                                     const Configuratie& configExterna)
    : Seif{nPlat},
      numePlatforma{nPlat},
      numeUtilizator{nUtiliz},
      parola{pass},
      config{configExterna}
{
}

void DateAutentificare::setter_parola(const std::string& nouaParola)
{
    // Folosim obiectul 'config'
    if (nouaParola.length() < config.getLungimeMinimaParola())
    {
        throw EroareValidare(
            numePlatforma,
            static_cast<int>(nouaParola.length()),
            static_cast<int>(config.getLungimeMinimaParola())
        );
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
    throw EroareValidare(numePlatforma);
}

void DateAutentificare::CriptareVigenere()
{
    std::string Cheie = config.getCheieVigenere();
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
    std::string Cheie = config.getCheieVigenere();
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