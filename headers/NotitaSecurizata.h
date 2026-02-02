#ifndef OOP_NOTITASECURIZATA_H
#define OOP_NOTITASECURIZATA_H
#include <string>

#include "Seif.h"


/**
 * @class NotitaSecurizata
 * @brief Represents a secured text note in the vault.
 */
class NotitaSecurizata : public Seif
{
    std::string notita; ///< The content of the note

public:
    /**
     * @brief Constructor for NotitaSecurizata.
     * @param eticheta Label for the note.
     * @param notita Content of the note.
     */
    NotitaSecurizata(const std::string& eticheta, const std::string& notita) : Seif{eticheta}, notita{notita}
    {
    };

    /**
     * @brief Creates a deep copy of the object.
     * @return A shared pointer to the cloned object.
     */
    [[nodiscard]] std::shared_ptr<Seif> clone() const override;

    /**
     * @brief Returns the type of the item.
     * @return String "Notita Securizata".
     */
    [[nodiscard]] std::string getTip() const override;

    /**
     * @brief Displays the note's details.
     * @param os Output stream.
     */
    void afiseaza(std::ostream& os) const override;

    /**
     * @brief Checks the security of the note.
     */
    void verificaSecuritate() const override;

    /**
     * @brief Updates the note's content.
     * @param nouContinut New content for the note.
     */
    void set_notita(const std::string& nouContinut);

    [[nodiscard]] const std::string& getNotita() const;

    /**
     * @brief Retrieves raw data fields for persistence.
     * @return Map with field names as keys and field values as values.
     */
    [[nodiscard]] std::map<std::string, std::string> getDatePentruSalvare() const override;
};


#endif //OOP_NOTITASECURIZATA_H
