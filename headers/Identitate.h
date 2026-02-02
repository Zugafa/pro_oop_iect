#ifndef OOP_IDENTITATE_H
#define OOP_IDENTITATE_H
#include "Seif.h"


/**
 * @class Identitate
 * @brief Represents identity information stored in the vault.
 * 
 * Stores personal details such as name, contact information, and address.
 */
class Identitate : public Seif
{
    std::string nume;       ///< Last name
    std::string prenume;    ///< First name
    std::string telefon;    ///< Phone number
    std::string email;      ///< Email address
    std::string strada;     ///< Street address
    std::string oras;       ///< City
    std::string judet;      ///< County/State
    std::string tara;       ///< Country
    std::string codPostal;  ///< Postal code

public:
    /**
     * @brief Detailed constructor for Identitate.
     * @param eticheta Label for the entry.
     * @param nume Last name.
     * @param prenume First name.
     * @param telefon Phone number.
     * @param email Email.
     * @param strada Street.
     * @param oras City.
     * @param judet County.
     * @param tara Country.
     * @param codPostal Postal code.
     */
    Identitate(const std::string& eticheta, const std::string& nume, const std::string& prenume,
               const std::string& telefon, const std::string& email, const std::string& strada, const std::string& oras,
               const std::string& judet,
               const std::string& tara, const std::string& codPostal);

    /**
     * @brief Creates a deep copy of the object.
     * @return A shared pointer to the cloned object.
     */
    [[nodiscard]] std::shared_ptr<Seif> clone() const override;

    /**
     * @brief Returns the type of the item.
     * @return String "Identitate".
     */
    [[nodiscard]] std::string getTip() const override;

    /**
     * @brief Displays the identity details.
     * @param os Output stream.
     */
    void afiseaza(std::ostream& os) const override;

    /**
     * @brief Checks the security of the identity entry.
     */
    void verificaSecuritate() const override;


    void set_nume(const std::string& nume);
    void set_prenume(const std::string& prenumeNou);
    void set_telefon(const std::string& telefonNou);
    void set_email(const std::string& emailNou);
    void set_strada(const std::string& strada);
    void set_oras(const std::string& orasNou);
    void set_judet(const std::string& judet);
    void set_tara(const std::string& tara);
    void set_cod_postal(const std::string& cod_postal);

    [[nodiscard]] const std::string& get_nume() const;
    [[nodiscard]] const std::string& get_prenume() const;
    [[nodiscard]] const std::string& get_telefon() const;
    [[nodiscard]] const std::string& get_email() const;
    [[nodiscard]] const std::string& get_strada() const;
    [[nodiscard]] const std::string& get_oras() const;
    [[nodiscard]] const std::string& get_judet() const;
    [[nodiscard]] const std::string& get_tara() const;
    [[nodiscard]] const std::string& get_cod_postal() const;

    /**
     * @brief Retrieves raw data fields for persistence.
     * @return Map with field names as keys and field values as values.
     */
    [[nodiscard]] std::map<std::string, std::string> getDatePentruSalvare() const override;
};

#endif //OOP_IDENTITATE_H
