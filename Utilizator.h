#ifndef OOP_UTILIZATOR_H
#define OOP_UTILIZATOR_H
#include "DateAutentificare.h"
#include "Seif.h"
#include <vector>
#include <memory>

class Utilizator
{
private:
    std::string nume, email, parolaMaster;
    std::vector<std::unique_ptr<Seif>> seif;

public:
    // Constructor de initializare
    explicit Utilizator(const std::string& n, const std::string& e, const std::string& pw)
        : nume{n}, email{e}, parolaMaster{pw}
    {
    }

    ~Utilizator() = default;

    // Constructor de copiere
    Utilizator(const Utilizator& sursa);

    // Operator de atribuire
    Utilizator& operator=(Utilizator sursa);

    void swap(Utilizator& other) noexcept;

    // operator<<
    friend std::ostream& operator<<(std::ostream& out, const Utilizator& user);

    [[nodiscard]] std::string getNume() const { return nume; }
    [[nodiscard]] std::string getEmail() const { return email; }

    [[nodiscard]] bool verificaParola(const std::string& parola) const;

    void adaugaObiect(std::unique_ptr<Seif> itemNou);
    void stergeObiect(const std::string& eticheta);
    [[nodiscard]] Seif* getObiectAt(int index) const;
};

// operator<< pentru Utilizator
std::ostream& operator<<(std::ostream& out, const Utilizator& user);

#endif //OOP_UTILIZATOR_H