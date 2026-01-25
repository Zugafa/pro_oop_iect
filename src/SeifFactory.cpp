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

std::shared_ptr<Seif> SeifFactory::creeazaSeif(const std::string& tip,
                                               const std::map<std::string, std::string>& date)
{
    if (tip == "DateAutentificare")
    {
        return std::make_shared<DateAutentificare>(
            date.at("platforma"),
            date.at("utilizator"),
            date.at("parola"),
            date.count("url") ? date.at("url") : "",
            date.count("note") ? date.at("note") : ""
        );
    }
    if (tip == "CardBancar")
    {
        return std::make_shared<CardBancar>(
            date.at("eticheta"),
            date.at("numar"),
            date.at("dataExpirare"),
            date.at("cvv"));
    }
    if (tip == "Identitate")
    {
        return std::make_shared<Identitate>(
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
        return std::make_shared<NotitaSecurizata>(
            date.at("eticheta"),
            date.at("notita"));
    }
    throw ObiectNegasit("SeifFactory: tipul de obiect din seif nu este valid!");
}
