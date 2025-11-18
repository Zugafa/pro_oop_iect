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
               const std::string& tara, const std::string& codPostal) : Seif{eticheta},
                                                                        nume{nume}, prenume{prenume}, telefon{telefon},
                                                                        email(email),
                                                                        strada{strada},
                                                                        oras{oras}, judet{judet}, tara{tara},
                                                                        codPostal{codPostal}
    {
    };

    [[nodiscard]] std::unique_ptr<Seif> clone() const override;

    [[nodiscard]] std::string getTip() const override;

    void afiseaza(std::ostream& os) const override;
};


#endif //OOP_IDENTITATE_H