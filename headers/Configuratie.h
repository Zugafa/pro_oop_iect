#ifndef OOP_CONFIGURATIE_H
#define OOP_CONFIGURATIE_H
#include <iostream>
#include <string>


/**
 * @class Configuratie
 * @brief Singleton class for application-wide security settings.
 * 
 * Stores the crypto key, valid special characters, and minimum password length.
 */
class Configuratie
{
    std::string cheieVigenere;          ///< Key used for the SHA3-based stream cipher
    std::string caractereSpecialeValide; ///< String containing allowed special characters
    std::size_t lungimeMinimaParola;    ///< Minimum required password length
    
    /**
     * @brief Private constructor for Singleton pattern.
     */
    Configuratie();

public:
    Configuratie(const Configuratie&) = delete;
    Configuratie& operator=(const Configuratie&) = delete;

    /**
     * @brief Retrieves the single instance of the Configuratie class.
     * @return Reference to the singleton instance.
     */
    static Configuratie& getInstance();

    [[nodiscard]] const std::string& getCheieVigenere() const { return cheieVigenere; }
    [[nodiscard]] const std::string& getCaractereSpecialeValide() const { return caractereSpecialeValide; }
    [[nodiscard]] std::size_t getLungimeMinimaParola() const { return lungimeMinimaParola; }

    ~Configuratie() = default;

    /**
     * @brief Overloads the << operator for Configuratie.
     * @param out Output stream.
     * @param config Configuratie object.
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& out, const Configuratie& config);
};

/**
 * @brief Global overload for << operator.
 */
std::ostream& operator<<(std::ostream& out, const Configuratie& config);


#endif //OOP_CONFIGURATIE_H
