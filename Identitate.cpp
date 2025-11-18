#include "Identitate.h"

std::unique_ptr<Seif> Identitate::clone() const
{
    return std::make_unique<Identitate>(*this);
}

std::string Identitate::getTip() const
{
    return "Identitate";
}

void Identitate::afiseaza(std::ostream& os) const
{
    os << "Nume: " << nume << std::endl << "Prenume: " << prenume << std::endl << "Numar telefon: " << telefon <<
        std::endl << "E-mail: " << email << std::endl << "Strada: " << strada << std::endl << "Oras: " << oras <<
        std::endl << "Judet: " << judet <<
        std::endl << "Tara: " << tara << std::endl << "Cod Postal: " << codPostal << std::endl;
}