#ifndef OOP_UTILIZATOR_H
#define OOP_UTILIZATOR_H
#include "DateAutentificare.h"
#include "Seif.h"
#include "Observer.h"
#include "Manager.h"
#include <vector>
#include <memory>

class Utilizator
{
private:
    std::string nume, email, parolaMaster;
    std::vector<std::shared_ptr<Seif>> seif;
    std::vector<Observer*> observatori;
    Manager<std::string> istoricActiuni;

public:
    explicit Utilizator(const std::string& n, const std::string& e, const std::string& pw)
        : nume{n}, email{e}, parolaMaster{pw}
    {
    }

    ~Utilizator() = default;

    Utilizator(const Utilizator& sursa);

    Utilizator& operator=(Utilizator sursa);

    void swap(Utilizator& other) noexcept;

    friend std::ostream& operator<<(std::ostream& out, const Utilizator& user);

    [[nodiscard]] const std::string& getNume() const { return nume; }
    [[nodiscard]] const std::string& getEmail() const { return email; }
    [[nodiscard]] bool verificaParola(const std::string& parola) const;

    void adaugaObiect(std::shared_ptr<Seif> itemNou);
    void stergeObiect(const std::string& eticheta);
    [[nodiscard]] Seif* getObiectAt(int index) const;
    [[nodiscard]] size_t getNrObiecte() const;

    void attach(Observer* obs);
    void notify() const;
};

std::ostream& operator<<(std::ostream& out, const Utilizator& user);

#endif //OOP_UTILIZATOR_H
