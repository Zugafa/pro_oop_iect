#ifndef OOP_SEIF_H
#define OOP_SEIF_H
#include <iostream>
#include <string>
#include <memory>

class Seif
{
    std::string eticheta;
    static int nrObiecteTotale;

public:
    explicit Seif(const std::string& eticheta);

    // Destructor virtual
    virtual ~Seif();

    // Functie statica
    static int getNumarObiecteTotale();

    // Interfata pura
    [[nodiscard]] virtual std::unique_ptr<Seif> clone() const = 0;
    [[nodiscard]] virtual std::string getTip() const = 0;
    [[nodiscard]] const std::string& getEticheta() const { return eticheta; }

    virtual void afiseaza(std::ostream& os) const = 0;

    // Functia specifica temei
    virtual void verificaSecuritate() const = 0;
};

// Operator afisare inline
inline std::ostream& operator<<(std::ostream& os, const Seif& s)
{
    s.afiseaza(os);
    return os;
}

#endif //OOP_SEIF_H