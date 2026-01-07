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
    Configuratie config; // Compunere

public:
    // Constructor de initializare
    explicit Utilizator(const std::string& n, const std::string& e, const std::string& pw,
                        const Configuratie& configExterna)
        : nume{n}, email{e}, config{configExterna}, parolaMaster{pw}
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

    std::string getNume() const { return nume; }
    std::string getEmail() const { return email; }

    [[nodiscard]] bool verificaParola(const std::string& parola) const;

    void adaugaObiect(std::unique_ptr<Seif> itemNou);
    void stergeObiect(const std::string& eticheta);
    [[nodiscard]] Seif* getObiectAt(int index) const;
};

// operator<< pentru Utilizator
std::ostream& operator<<(std::ostream& out, const Utilizator& user);

#endif //OOP_UTILIZATOR_H