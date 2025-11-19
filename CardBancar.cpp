//
// Created by nitui on 11/14/2025.
//
#include <iostream>
#include "CardBancar.h"

std::unique_ptr<Seif> CardBancar::clone() const
{
    return std::make_unique<CardBancar>(*this);
}

std::string CardBancar::getTip() const
{
    return "Card Bancar";
}

void CardBancar::afiseaza(std::ostream& os) const
{
    os << "Card Bancar: " << numar << std::endl;
}

void CardBancar::verificaSecuritate() const
{
    if (cvv.length() != 3)
    {
        std::cout << "[ALERT] Cardul '" << getEticheta() << "': Codul CVV pare invalid (" << cvv.length() <<
            " cifre).\n";
    }
    else
    {
        std::cout << "[SECURE] Cardul '" << getEticheta() << "' are formatul de securitate valid.\n";
    }
}