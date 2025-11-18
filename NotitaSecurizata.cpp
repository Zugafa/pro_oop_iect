#include "NotitaSecurizata.h"

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