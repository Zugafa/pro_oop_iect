//
// Created by nitui on 11/14/2025.
//

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