#ifndef OOP_SEIF_H
#define OOP_SEIF_H
#include <iostream>
#include <string>
#include <memory>

class Seif
{
    std::string eticheta;

public:
    Seif(const std::string& eticheta) : eticheta(eticheta)
    {
    };
    virtual ~Seif() = default;

    virtual std::unique_ptr<Seif> clone() const =0;
    virtual std::string getTip() const =0;
    [[nodiscard]] const std::string& getEticheta() const { return eticheta; }
    virtual void afiseaza(std::ostream& os) const = 0;
};

inline std::ostream& operator<<(std::ostream&, const Seif&);

#endif //OOP_SEIF_H