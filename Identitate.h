#ifndef OOP_IDENTITATE_H
#define OOP_IDENTITATE_H
#include "Seif.h"


class Identitate : public Seif
{
    std::string nume, prenume, telefon, email, strada, oras, judet, tara, codPostal;

public:
    Identitate(const std::string& eticheta, const std::string& nume, const std::string& prenume,
               const std::string& telefon, const std::string& email, const std::string& strada, const std::string& oras,
               const std::string& judet,
               const std::string& tara, const std::string& codPostal);

    [[nodiscard]] std::unique_ptr<Seif> clone() const override;

    [[nodiscard]] std::string getTip() const override;

    void afiseaza(std::ostream& os) const override;
    void verificaSecuritate() const override;
};


#endif //OOP_IDENTITATE_H