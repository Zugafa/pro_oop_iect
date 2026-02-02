#ifndef OOP_UTILIZATOR_H
#define OOP_UTILIZATOR_H
#include "DateAutentificare.h"
#include "Seif.h"
#include "Observer.h"
#include "Manager.h"
#include <vector>
#include <memory>

/**
 * @class Utilizator
 * @brief Represents a user of the application.
 * 
 * Stores user profile information and a collection of vault items (Seif).
 * Implements the Subject part of the Observer pattern to notify listeners of changes.
 */
class Utilizator
{
private:
    std::string nume;                        ///< User name
    std::string email;                       ///< User email
    std::string parolaMaster;                ///< Master password for authentication
    std::vector<std::shared_ptr<Seif>> seif; ///< Collection of items in the vault
    std::vector<Observer*> observatori;      ///< Registered observers
    Manager<std::string> istoricActiuni;      ///< History of actions performed by the user

public:
    /**
     * @brief Detailed constructor for Utilizator.
     * @param n Name.
     * @param e Email.
     * @param pw Master password.
     */
    explicit Utilizator(const std::string& n, const std::string& e, const std::string& pw)
        : nume{n}, email{e}, parolaMaster{pw}
    {
    }

    ~Utilizator() = default;

    /**
     * @brief Copy constructor performing a deep copy of the vault items.
     * @param sursa Source object.
     */
    Utilizator(const Utilizator& sursa);

    /**
     * @brief Assignment operator using copy-and-swap idiom.
     * @param sursa Source object.
     * @return Reference to this object.
     */
    Utilizator& operator=(Utilizator sursa);

    /**
     * @brief Swaps the contents of two Utilizator objects.
     * @param other The other object.
     */
    void swap(Utilizator& other) noexcept;

    /**
     * @brief Overloads the << operator for Utilizator.
     * @param out Output stream.
     * @param user Utilizator object.
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& out, const Utilizator& user);

    [[nodiscard]] const std::string& getNume() const { return nume; }
    [[nodiscard]] const std::string& getEmail() const { return email; }

    /**
     * @brief Verifies if the provided password matches the master password.
     * @param parola Password to check.
     * @return True if they match, false otherwise.
     */
    [[nodiscard]] bool verificaParola(const std::string& parola) const;

    /**
     * @brief Adds a new item to the user's vault in sorted order.
     * @param itemNou Shared pointer to the new item.
     */
    void adaugaObiect(std::shared_ptr<Seif> itemNou);

    /**
     * @brief Removes an item from the vault by its label.
     * @param eticheta Label of the item to remove.
     * @throws ObiectNegasit if the item is not found.
     */
    void stergeObiect(const std::string& eticheta);

    /**
     * @brief Retrieves an item at a specific index.
     * @param index Index in the vault.
     * @return Pointer to the item.
     * @throws EroareAcces if index is out of bounds.
     */
    [[nodiscard]] Seif* getObiectAt(int index) const;

    /**
     * @brief Returns the number of items in the vault.
     * @return Item count.
     */
    [[nodiscard]] size_t getNrObiecte() const;

    /**
     * @brief Attaches an observer to the user.
     * @param obs Pointer to the observer.
     */
    void attach(Observer* obs);

    /**
     * @brief Notifies all attached observers of a change.
     */
    void notify() const;
};

std::ostream& operator<<(std::ostream& out, const Utilizator& user);

#endif //OOP_UTILIZATOR_H
