#include "NotitaSecurizata.h"
#include <iostream>

std::unique_ptr<Seif> NotitaSecurizata::clone() const
{
    return std::make_unique<NotitaSecurizata>(*this);
}

std::string NotitaSecurizata::getTip() const
{
    return "Notita Securizata";
}

void NotitaSecurizata::afiseaza(std::ostream& os) const
{
    os << "Notita Securizata: " << notita << std::endl;
}

void NotitaSecurizata::verificaSecuritate() const
{
    if (notita.empty())
    {
        std::cout << "[INFO] Notita '" << getEticheta() << "' este goala.\n";
    }
    else
    {
        std::cout << "[LOCKED] Notita '" << getEticheta() << "' contine date criptate.\n";
    }
}