#ifndef OOP_DATEAUTENTIFICARE_H
#define OOP_DATEAUTENTIFICARE_H
/**
 * @file DateAutentificare.h
 * @brief Declares the DateAutentificare class for storing platform credentials.
 */
#include "Configuratie.h"
#include "Seif.h"

/**
 * @class DateAutentificare
 * @brief Represents authentication data (credentials) for a platform.
 *
 * Inherits from Seif and provides fields for platform name, username, password,
 * URL, and notes. Includes methods for password validation and a SHA3-derived
 * stream cipher for protecting the stored password.
 */
class DateAutentificare : public Seif
{
    std::string utilizator;      ///< Username
    std::string parola;          ///< Password
    std::string url;             ///< URL of the platform
    std::string numePlatforma;   ///< Platform name (same as label)
    std::string note;            ///< Additional notes

public:
    /**
     * @brief Detailed constructor for DateAutentificare.
     * @param eticheta_ Platform/Label name.
     * @param utilizator_ Username.
     * @param parola_ Password.
     * @param url_ Platform URL.
     * @param note_ Additional notes.
     */
    DateAutentificare(const std::string& eticheta_,
                      const std::string& utilizator_,
                      const std::string& parola_,
                      const std::string& url_,
                      const std::string& note_);

    /**
     * @brief Constructor that initializes from a map of strings.
     * @param date Map containing keys "platforma", "utilizator", "parola", "url", "note".
     * @throws std::out_of_range if any required key is missing.
     */
    explicit DateAutentificare(const std::map<std::string, std::string>& date);
    DateAutentificare(const DateAutentificare& sursa) = default;
    DateAutentificare& operator=(const DateAutentificare& sursa) = default;
    ~DateAutentificare() override = default;

    /**
     * @brief Creates a deep copy of the object.
     * @return A shared pointer to the cloned object.
     */
    [[nodiscard]] std::shared_ptr<Seif> clone() const override
    {
        return std::make_shared<DateAutentificare>(*this);
    };

    /**
     * @brief Returns the type of the object.
     * @return String "Date Autentificare".
     */
    [[nodiscard]] std::string getTip() const override
    {
        return "Date Autentificare";
    };

    /**
     * @brief Sets a new label for the entry and updates platform name.
     * @param eticheta The new label string.
     */
    void setEticheta(const std::string& eticheta) override;

    /**
     * @brief Displays the authentication data to the output stream.
     * @param os Output stream.
     */
    void afiseaza(std::ostream& os) const override
    {
        os << "  -> Platform: " << numePlatforma << " | User: " << utilizator <<
            " | Current Password: [" << parola << "]\n";
    };

    [[nodiscard]] const std::string& getPlatforma() const { return numePlatforma; }
    [[nodiscard]] const std::string& getUtilizator() const { return utilizator; }
    [[nodiscard]] const std::string& getParola() const { return parola; }
    [[nodiscard]] const std::string& getUrl() const { return url; }
    [[nodiscard]] const std::string& getNote() const { return note; }

    /**
     * @brief Updates the password with validation.
     * @param nouaParola The new password to set.
     * @throws EroareValidare if the password does not meet security requirements.
     */
    void setter_parola(const std::string& nouaParola);

    /**
     * @brief Updates the username.
     * @param nouNume New username.
     */
    void setter_numeUtilizator(const std::string& nouNume);

    /**
     * @brief Updates the platform name.
     * @param nouNume New platform name.
     */
    void setter_numePlatforma(const std::string& nouNume);

    /**
     * @brief Updates the URL.
     * @param nouaUrl New URL.
     */
    void setter_url(const std::string& nouaUrl);

    /**
     * @brief Updates the notes.
     * @param nouaNote New notes.
     */
    void setter_note(const std::string& nouaNote);

    /**
     * @brief Protects the password with a SHA3-derived XOR stream cipher.
     */
    void CriptareVigenere();

    /**
     * @brief Reverses the SHA3-derived stream cipher.
     */
    void deCriptareVigenere();

    /**
     * @brief Checks and prints the security level of the password.
     *
     * Emits a message to std::cout based on the password length.
     */
    void verificaSecuritate() const override;

    /**
     * @brief Retrieves a map of all fields for persistence.
     * @return Map with field names as keys and field values as values.
     */
    [[nodiscard]] std::map<std::string, std::string> getDatePentruSalvare() const override;
};

#endif //OOP_DATEAUTENTIFICARE_H
