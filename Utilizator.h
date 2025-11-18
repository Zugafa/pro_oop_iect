#ifndef OOP_UTILIZATOR_H
#define OOP_UTILIZATOR_H
#include "DateAutentificare.h"
#include "Seif.h"
#include <vector>
#include <memory>
// -------------------------------------------------------------
// CLASA Utilizator (Clasa #2, Compunere)
// -------------------------------------------------------------
class Utilizator
{
private:
    std::string nume, email;
    std::vector<std::unique_ptr<Seif>> seif;
    Configuratie config; // Compunere

public:
    // Constructor de initializare
    explicit Utilizator(const std::string& n, const std::string& e, const Configuratie& configExterna)
        : nume{n}, email{e}, config{configExterna}
    {
    }

    ~Utilizator() = default;

    // Constructor de copiere
    Utilizator(const Utilizator& sursa);

    // Operator de atribuire
    Utilizator& operator=(Utilizator sursa);

    void swap(Utilizator& other);

    // operator<<
    friend std::ostream& operator<<(std::ostream& out, const Utilizator& user);

    void adaugaObiect(std::unique_ptr<Seif> itemNou);
    void stergeObiect(const std::string& platforma, const std::string& username);
};

// operator<< pentru Utilizator
std::ostream& operator<<(std::ostream& out, const Utilizator& user);

#endif //OOP_UTILIZATOR_H