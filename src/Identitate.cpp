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

std::shared_ptr<Seif> Identitate::clone() const
{
    return std::make_shared<Identitate>(*this);
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

void Identitate::set_nume(const std::string& numeNou)
{
    nume = numeNou;
}

void Identitate::set_prenume(const std::string& prenumeNou)
{
    prenume = prenumeNou;
}

void Identitate::set_telefon(const std::string& telefonNou)
{
    telefon = telefonNou;
}

void Identitate::set_email(const std::string& emailNou)
{
    email = emailNou;
}

void Identitate::set_strada(const std::string& stradaNoua)
{
    strada = stradaNoua;
}

void Identitate::set_oras(const std::string& orasNou)
{
    oras = orasNou;
}

void Identitate::set_judet(const std::string& judetNou)
{
    judet = judetNou;
}

void Identitate::set_tara(const std::string& taraNoua)
{
    tara = taraNoua;
}

void Identitate::set_cod_postal(const std::string& cod_postal)
{
    codPostal = cod_postal;
}

const std::string& Identitate::get_nume() const
{
    return nume;
}

const std::string& Identitate::get_prenume() const
{
    return prenume;
}

const std::string& Identitate::get_telefon() const
{
    return telefon;
}

const std::string& Identitate::get_email() const
{
    return email;
}

const std::string& Identitate::get_strada() const
{
    return strada;
}

const std::string& Identitate::get_oras() const
{
    return oras;
}

const std::string& Identitate::get_judet() const
{
    return judet;
}

const std::string& Identitate::get_tara() const
{
    return tara;
}

const std::string& Identitate::get_cod_postal() const
{
    return codPostal;
}

std::map<std::string, std::string> Identitate::getDatePentruSalvare() const
{
    std::map<std::string, std::string> date;
    date["eticheta"] = getEticheta();
    date["nume"] = nume;
    date["prenume"] = prenume;
    date["telefon"] = telefon;
    date["email"] = email;
    date["strada"] = strada;
    date["oras"] = oras;
    date["judet"] = judet;
    date["tara"] = tara;
    date["codPostal"] = codPostal;
    return date;
}
