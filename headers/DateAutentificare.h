#ifndef OOP_DATEAUTENTIFICARE_H
#define OOP_DATEAUTENTIFICARE_H
#include "Configuratie.h"
#include "Seif.h"

class DateAutentificare : public Seif
{
    std::string utilizator;
    std::string parola;
    std::string url;
    std::string numePlatforma;
    std::string note;

public:
    DateAutentificare(const std::string& eticheta_,
                      const std::string& utilizator_,
                      const std::string& parola_,
                      const std::string& url_,
                      const std::string& note_);

    explicit DateAutentificare(const std::map<std::string, std::string>& date);
    DateAutentificare(const DateAutentificare& sursa) = default;
    DateAutentificare& operator=(const DateAutentificare& sursa) = default;
    ~DateAutentificare() override = default;

    [[nodiscard]] std::shared_ptr<Seif> clone() const override
    {
        return std::make_shared<DateAutentificare>(*this);
    };

    [[nodiscard]] std::string getTip() const override
    {
        return "Date Autentificare";
    };
    void setEticheta(const std::string& eticheta) override;

    void afiseaza(std::ostream& os) const override
    {
        os << "  -> Platforma: " << numePlatforma << " | Utilizator: " << utilizator <<
            " | Parola Curenta: [" << parola << "]\n";
    };

    // Getteri const
    [[nodiscard]] const std::string& getPlatforma() const { return numePlatforma; }
    [[nodiscard]] const std::string& getUtilizator() const { return utilizator; }
    [[nodiscard]] const std::string& getParola() const { return parola; }

    // 1. Setter Parola cu Validare
    void setter_parola(const std::string& nouaParola);
    void setter_numeUtilizator(const std::string& nouNume);

    // 2. Criptare Vigenere
    void CriptareVigenere();

    // 3. Decriptare Vigenere
    void deCriptareVigenere();

    void verificaSecuritate() const override;

    [[nodiscard]] std::map<std::string, std::string> getDatePentruSalvare() const override;
};

#endif //OOP_DATEAUTENTIFICARE_H
