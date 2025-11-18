#ifndef OOP_DATEAUTENTIFICARE_H
#define OOP_DATEAUTENTIFICARE_H
#include "Configuratie.h"
#include "Seif.h"
// =============================================================
// CLASA DateAutentificare (Clasa #1)
// =============================================================
class DateAutentificare : public Seif
{
private:
    std::string numePlatforma, numeUtilizator, parola;
    Configuratie config; // compunere de clase

public:
    // Constructor explicit de initializare (cu compunere)
    explicit DateAutentificare(const std::string& eticheta, const std::string& nPlat, const std::string& nUtiliz,
                               const std::string& pass,
                               const Configuratie& configExterna)
        :
        Seif{eticheta},
        numePlatforma{nPlat},
        numeUtilizator{nUtiliz},
        parola{pass},
        config{configExterna} // init config
    {
    }

    DateAutentificare(const DateAutentificare& sursa) = default;
    DateAutentificare& operator=(const DateAutentificare& sursa) = default;
    ~DateAutentificare() = default;

    [[nodiscard]] std::unique_ptr<Seif> clone() const override
    {
        return std::make_unique<DateAutentificare>(*this);
    };

    [[nodiscard]] std::string getTip() const override
    {
        return "Date Autentificare";
    };

    void afiseaza(std::ostream& os) const override
    {
        os << "  -> Platforma: " << numePlatforma << " | Utilizator: " << numeUtilizator <<
            " | Parola Curenta: [" << parola << "]\n";
    };

    // Getteri const
    [[nodiscard]] const std::string& getPlatforma() const { return numePlatforma; }
    [[nodiscard]] const std::string& getUtilizator() const { return numeUtilizator; }
    [[nodiscard]] const std::string& getParola() const { return parola; }
    // Getter pentru config (necesar pt adaugaCont)
    [[nodiscard]] const Configuratie& getConfig() const { return config; }

    // 1. Setter Parola cu Validare
    void setter_parola(const std::string& nouaParola);

    // 2. Criptare Vigenere
    void CriptareVigenere();

    // 3. Decriptare Vigenere
    void deCriptareVigenere();
};

#endif //OOP_DATEAUTENTIFICARE_H