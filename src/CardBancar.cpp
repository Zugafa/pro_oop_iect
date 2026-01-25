#include <iostream>
#include "CardBancar.h"

std::shared_ptr<Seif> CardBancar::clone() const
{
    return std::make_shared<CardBancar>(*this);
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

void CardBancar::setNumar(const std::string& n)
{
    this->numar = n;
}

void CardBancar::setDataExp(const std::string& d)
{
    this->dataExpirare = d;
}

void CardBancar::setCVV(const std::string& c)
{
    this->cvv = c;
}

const std::string& CardBancar::getNumar() const
{
    return numar;
}

const std::string& CardBancar::getDataExp() const
{
    return dataExpirare;
}

const std::string& CardBancar::getCVV() const
{
    return cvv;
}

std::map<std::string, std::string> CardBancar::getDatePentruSalvare() const
{
    std::map<std::string, std::string> date;
    date["eticheta"] = getEticheta();
    date["numar"] = numar;
    date["dataExpirare"] = dataExpirare;
    date["cvv"] = cvv;
    return date;
}
