#include "Identitate.h"
#include <iostream>

/**
 * @brief Detailed constructor.
 */
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

/**
 * @brief Creates a deep copy of the Identitate object.
 */
std::shared_ptr<Seif> Identitate::clone() const
{
    return std::make_shared<Identitate>(*this);
}

/**
 * @brief Returns the type label.
 */
std::string Identitate::getTip() const
{
    return "Identitate";
}

/**
 * @brief Displays identity details.
 */
void Identitate::afiseaza(std::ostream& os) const
{
    os << "Last Name: " << nume << std::endl << "First Name: " << prenume << std::endl << "Phone: " << telefon <<
        std::endl << "E-mail: " << email << std::endl << "Street: " << strada << std::endl << "City: " << oras <<
        std::endl << "State/County: " << judet <<
        std::endl << "Country: " << tara << std::endl << "Postal Code: " << codPostal << std::endl;
}

/**
 * @brief Prints a message to verify if identity data is up to date.
 */
void Identitate::verificaSecuritate() const
{
    std::cout << "[INFO] Identity '" << getEticheta() << "': Please verify if the data (Phone: "
        << telefon << ") is up to date.\n";
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

/**
 * @brief Returns identity fields as a map for saving.
 */
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
