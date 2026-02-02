#ifndef OOP_SEIF_H
#define OOP_SEIF_H
#include <iostream>
#include <string>
#include <memory>
#include <map>

/**
 * @class Seif
 * @brief Base class for items stored in the vault.
 * 
 * Provides basic functionality for items such as labeling, cloning, and serialization.
 * Tracks the total number of Seif objects.
 */
class Seif
{
    std::string eticheta;      ///< Label or identifier for the item
    static int nrObiecteTotale; ///< Static counter for total number of objects

public:
    /**
     * @brief Constructor for Seif.
     * @param eticheta The label for the item.
     */
    explicit Seif(const std::string& eticheta);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Seif();

    /**
     * @brief Gets the total number of Seif objects currently in existence.
     * @return Total count of objects.
     */
    static int getNumarObiecteTotale();

    /**
     * @brief Creates a deep copy of the object.
     * @return A shared pointer to the cloned object.
     */
    [[nodiscard]] virtual std::shared_ptr<Seif> clone() const = 0;

    /**
     * @brief Returns the type of the item.
     * @return String representing the type.
     */
    [[nodiscard]] virtual std::string getTip() const = 0;

    /**
     * @brief Gets the label of the item.
     * @return The label string.
     */
    [[nodiscard]] const std::string& getEticheta() const;

    /**
     * @brief Sets a new label for the item.
     * @param eticheta The new label string.
     */
    virtual void setEticheta(const std::string& eticheta);

    /**
     * @brief Displays the item's details to an output stream.
     * @param os The output stream.
     */
    virtual void afiseaza(std::ostream& os) const = 0;

    /**
     * @brief Checks the security of the item.
     */
    virtual void verificaSecuritate() const = 0;

    /**
     * @brief Retrieves raw data fields for persistence.
     * @return Map with field names as keys and field values as values.
     */
    [[nodiscard]] virtual std::map<std::string, std::string> getDatePentruSalvare() const = 0;
};

/**
 * @brief Overloads the << operator for Seif objects.
 * @param os Output stream.
 * @param s Seif object.
 * @return Reference to the output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const Seif& s)
{
    s.afiseaza(os);
    return os;
}

#endif //OOP_SEIF_H