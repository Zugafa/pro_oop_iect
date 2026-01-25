#ifndef OOP_NOTITASECURIZATA_H
#define OOP_NOTITASECURIZATA_H
#include <string>

#include "Seif.h"


class NotitaSecurizata : public Seif
{
    std::string notita;

public:
    NotitaSecurizata(const std::string& eticheta, const std::string& notita) : Seif{eticheta}, notita{notita}
    {
    };

    [[nodiscard]] std::shared_ptr<Seif> clone() const override;

    [[nodiscard]] std::string getTip() const override;

    void afiseaza(std::ostream& os) const override;
    void verificaSecuritate() const override;

    void set_notita(const std::string& nouContinut);
    [[nodiscard]] const std::string& getNotita() const;

    [[nodiscard]] std::map<std::string, std::string> getDatePentruSalvare() const override;
};


#endif //OOP_NOTITASECURIZATA_H
