#ifndef OOP_CARDBANCAR_H
#define OOP_CARDBANCAR_H
#include "Seif.h"


/**
 * @class CardBancar
 * @brief Represents a bank card stored in the vault.
 *
 * Inherits from Seif and provides fields for card number, cardholder name,
 * expiration date, and CVV.
 */
class CardBancar : public Seif
{
    std::string numar;          ///< Card number
    std::string dataExpirare;   ///< Expiration date (MM/YY)
    std::string cvv;            ///< CVV code
    std::string numeDetinator; ///< Cardholder Name

public:
    /**
     * @brief Constructor for CardBancar.
     * @param eticheta Label for the card.
     * @param numeDetinator Cardholder's name.
     * @param numar Card number.
     * @param dataExpirare Expiration date.
     * @param cvv CVV code.
     */
    CardBancar(const std::string& eticheta,const std::string& numeDetinator, const std::string& numar, const std::string& dataExpirare,
               const std::string& cvv)
        : Seif{eticheta}, numar{numar}, dataExpirare{dataExpirare}, cvv{cvv}, numeDetinator{numeDetinator}
    {
    }

    /**
     * @brief Creates a deep copy of the object.
     * @return A shared pointer to the cloned object.
     */
    [[nodiscard]] std::shared_ptr<Seif> clone() const override;

    /**
     * @brief Returns the type of the item.
     * @return String "Card Bancar".
     */
    [[nodiscard]] std::string getTip() const override;

    /**
     * @brief Displays the card's details.
     * @param os Output stream.
     */
    void afiseaza(std::ostream& os) const override;

    /**
     * @brief Checks the security of the card (e.g., CVV length).
     */
    void verificaSecuritate() const override;

    /**
     * @brief Updates the card number.
     * @param n New card number.
     */
    void setNumar(const std::string& n);

    /**
     * @brief Updates the expiration date.
     * @param d New expiration date.
     */
    void setDataExp(const std::string& d);

    /**
     * @brief Updates the CVV code.
     * @param c New CVV code.
     */
    void setCVV(const std::string& c);

    /**
     * @brief Updates the cardholder's name.
     * @param nD New cardholder name.
     */
    void setNumeDetinator(const std::string& nD);

    /**
     * @brief Returns the card number.
     */
    [[nodiscard]] const std::string& getNumar() const;
    /**
     * @brief Returns the expiration date (MM/YY).
     */
    [[nodiscard]] const std::string& getDataExp() const;
    /**
     * @brief Returns the CVV code.
     */
    [[nodiscard]] const std::string& getCVV() const;
    /**
     * @brief Returns the cardholder's name.
     */
    [[nodiscard]] const std::string& getNumeDetinator() const;

    /**
     * @brief Retrieves raw data fields for persistence.
     * @return Map with field names as keys and field values as values.
     * @note Includes the "numeDetinator" field for the cardholder name.
     */
    [[nodiscard]] std::map<std::string, std::string> getDatePentruSalvare() const override;
};


#endif //OOP_CARDBANCAR_H
