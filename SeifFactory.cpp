#include "SeifFactory.h"
#include "DateAutentificare.h"
#include "CardBancar.h"
#include "Identitate.h"
#include "NotitaSecurizata.h"
#include "ObiectNegasit.h"

SeifFactory& SeifFactory::getInstance()
{
    static SeifFactory instance;
    return instance;
}

std::unique_ptr<Seif> SeifFactory::creeazaSeif(const std::string& tip,
                                               const std::map<std::string, std::string>& date)
{
    if (tip == "DateAutentificare")
    {
        return std::make_unique<DateAutentificare>(
            date.at("platforma"),
            date.at("utilizator"),
            date.at("parola")
        );
    }
    if (tip == "CardBancar")
    {
        return std::make_unique<CardBancar>(
            date.at("eticheta"),
            date.at("numar"),
            date.at("dataExpirare"),
            date.at("cvv"));
    }
    if (tip == "Identitate")
    {
        return std::make_unique<Identitate>(
            date.at("eticheta"),
            date.at("nume"),
            date.at("prenume"),
            date.at("telefon"),
            date.at("email"),
            date.at("strada"),
            date.at("oras"),
            date.at("judet"),
            date.at("tara"),
            date.at("codPostal"));
    }
    if (tip == "NotitaSecurizata")
    {
        return std::make_unique<NotitaSecurizata>(
            date.at("eticheta"),
            date.at("notita"));
    }
    throw ObiectNegasit("SeifFactory: tipul de obiect din seif nu este valid!");
}
