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

    [[nodiscard]] std::shared_ptr<Seif> clone() const override;

    [[nodiscard]] std::string getTip() const override;

    void afiseaza(std::ostream& os) const override;
    void verificaSecuritate() const override;


    void set_nume(const std::string& nume);
    void set_prenume(const std::string& prenume);
    void set_telefon(const std::string& telefon);
    void set_email(const std::string& email);
    void set_strada(const std::string& strada);
    void set_oras(const std::string& oras);
    void set_judet(const std::string& judet);
    void set_tara(const std::string& tara);
    void set_cod_postal(const std::string& cod_postal);

    [[nodiscard]] const std::string& get_nume() const;
    [[nodiscard]] const std::string& get_prenume() const;
    [[nodiscard]] const std::string& get_telefon() const;
    [[nodiscard]] const std::string& get_email() const;
    [[nodiscard]] const std::string& get_strada() const;
    [[nodiscard]] const std::string& get_oras() const;
    [[nodiscard]] const std::string& get_judet() const;
    [[nodiscard]] const std::string& get_tara() const;
    [[nodiscard]] const std::string& get_cod_postal() const;

    [[nodiscard]] std::map<std::string, std::string> getDatePentruSalvare() const override;
};

#endif //OOP_IDENTITATE_H
