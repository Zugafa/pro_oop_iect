//
// Created by nitui on 11/2/2025.
//

#ifndef OOP_CONFIGURATIE_H
#define OOP_CONFIGURATIE_H
#include <iostream>
#include <string>


class Configuratie
{
    std::string cheieVigenere, caractereSpecialeValide;
    std::size_t lungimeMinimaParola;

public:
    // Constructor de initializare cu valori default
    Configuratie();

    // Gettere const
    [[nodiscard]] const std::string& getCheieVigenere() const { return cheieVigenere; }
    [[nodiscard]] const std::string& getCaractereSpecialeValide() const { return caractereSpecialeValide; }
    [[nodiscard]] std::size_t getLungimeMinimaParola() const { return lungimeMinimaParola; }

    ~Configuratie() = default;

    // operator<<
    friend std::ostream& operator<<(std::ostream& out, const Configuratie& config);
};

// operator<< pentru Configuratie
std::ostream& operator<<(std::ostream& out, const Configuratie& config);


#endif //OOP_CONFIGURATIE_H