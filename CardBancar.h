#ifndef OOP_CARDBANCAR_H
#define OOP_CARDBANCAR_H
#include "Seif.h"


class CardBancar : public Seif
{
    std::string numar, dataExpirare, cvv;

public:
    CardBancar(const std::string& eticheta, const std::string& numar, const std::string& dataExpirare,
               const std::string& cvv) : Seif{eticheta}, numar{numar}, dataExpirare{dataExpirare}, cvv{cvv}
    {
    }

    [[nodiscard]] std::unique_ptr<Seif> clone() const override;

    [[nodiscard]] std::string getTip() const override;


    void afiseaza(std::ostream& os) const override;
    void verificaSecuritate() const override;

    void setNumar(const std::string& n);
    void setDataExp(const std::string& d);
    void setCVV(const std::string& c);

    [[nodiscard]] const std::string& getNumar() const;
    [[nodiscard]] const std::string& getDataExp() const;
    [[nodiscard]] const std::string& getCVV() const;
};


#endif //OOP_CARDBANCAR_H
