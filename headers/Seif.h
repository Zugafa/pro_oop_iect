#ifndef OOP_SEIF_H
#define OOP_SEIF_H
#include <iostream>
#include <string>
#include <memory>
#include <map>

class Seif
{
    std::string eticheta;
    static int nrObiecteTotale;

public:
    explicit Seif(const std::string& eticheta);

    virtual ~Seif();

    static int getNumarObiecteTotale();

    [[nodiscard]] virtual std::shared_ptr<Seif> clone() const = 0;
    [[nodiscard]] virtual std::string getTip() const = 0;
    [[nodiscard]] const std::string& getEticheta() const;
    virtual void setEticheta(const std::string& eticheta);

    virtual void afiseaza(std::ostream& os) const = 0;

    virtual void verificaSecuritate() const = 0;

    // Returnează datele brute pentru salvare
    [[nodiscard]] virtual std::map<std::string, std::string> getDatePentruSalvare() const = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Seif& s)
{
    s.afiseaza(os);
    return os;
}

#endif //OOP_SEIF_H