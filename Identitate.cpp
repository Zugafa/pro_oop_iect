#include "Identitate.h"
#include <iostream>

Identitate::Identitate(const std::string& eticheta, const std::string& nume, const std::string& prenume,
                       const std::string& telefon, const std::string& email, const std::string& strada,
                       const std::string& oras,
                       const std::string& judet, const std::string& tara,
                       const std::string& codPostal) : Seif{eticheta},
                                                       nume{nume}, prenume{prenume}, telefon{telefon},
                                                       email(email),
                                                       strada{strada},
                                                       oras{oras}, judet{judet}, tara{tara},
                                                       codPostal{codPostal}
{
}

std::unique_ptr<Seif> Identitate::clone() const
{
    return std::make_unique < Identitate > (*this);
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

void Identitate::verificaSecuritate() const
{
    std::cout << "[INFO] Identitatea '" << getEticheta() << "': Verifica daca datele (Telefon: "
        << telefon << ") sunt actualizate.\n";
}