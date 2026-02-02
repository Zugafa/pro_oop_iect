#ifndef OOP_GESTIONARPAROLE_H
#define OOP_GESTIONARPAROLE_H
#include "Manager.h"
#include "Utilizator.h"
#include <map>

#include "SeifFactory.h"

/**
 * @class GestionarParole
 * @brief Singleton class that coordinates users and vault operations.
 * 
 * Acts as the main controller for the application, handling logins, data persistence,
 * and interactions between users and their vaults.
 */
class GestionarParole
{
    Manager<Utilizator> utilizatori; ///< Manager for all registered users
    Utilizator* user_curent;         ///< Pointer to the currently logged-in user

    /**
     * @brief Private constructor for Singleton pattern.
     */
    GestionarParole() : user_curent(nullptr)
    {
    }

    GestionarParole(const GestionarParole& other) = delete;
    GestionarParole& operator=(const GestionarParole& other) = delete;

public:
    /**
     * @brief Returns the single instance of the GestionarParole class.
     * @return Reference to the singleton instance.
     */
    static GestionarParole& getInstance();

    /**
     * @brief Returns the manager of users.
     * @return Reference to the user manager.
     */
    Manager<Utilizator>& getManager() { return utilizatori; }

    /**
     * @brief Authenticates a user.
     * @param numeUtilizator Username.
     * @param parola Password.
     * @throws ObiectNegasit if the user doesn't exist.
     * @throws ScriptException if the password is incorrect.
     */
    void login(const std::string& numeUtilizator, const std::string& parola);

    /**
     * @brief Logs out the current user.
     */
    void logout() { user_curent = nullptr; }

    /**
     * @brief Returns the currently logged-in user.
     * @return Pointer to the current user, or nullptr if no one is logged in.
     */
    [[nodiscard]] Utilizator* getUserCrt() const;

    /**
     * @brief Adds a new vault item for the current user.
     * @param tip Type of the item (e.g., "Card Bancar", "Date Autentificare").
     * @param date Map of field names and values for the item.
     */
    void adaugaObiectInSeif(const std::string& tip, const std::map<std::string, std::string>& date) const;

    /**
     * @brief Saves all application data (users and their vaults) to disk.
     */
    void salveazaDatelePeDisc() const;

    /**
     * @brief Loads application data from disk.
     */
    void incarcaDateleDePeDisc() const;
};


#endif //OOP_GESTIONARPAROLE_H
