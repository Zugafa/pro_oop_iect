#include "NotitaSecurizata.h"
#include <iostream>

/**
 * @brief Creates a deep copy of the NotitaSecurizata object.
 */
std::shared_ptr<Seif> NotitaSecurizata::clone() const
{
    return std::make_shared<NotitaSecurizata>(*this);
}

/**
 * @brief Returns the type label.
 */
std::string NotitaSecurizata::getTip() const
{
    return "Notita Securizata";
}

/**
 * @brief Displays the note's content.
 */
void NotitaSecurizata::afiseaza(std::ostream& os) const
{
    os << "Secure Note: " << notita << std::endl;
}

/**
 * @brief Prints the security status of the note.
 */
void NotitaSecurizata::verificaSecuritate() const
{
    if (notita.empty())
        std::cout << "[INFO] Note '" << getEticheta() << "' is empty.\n";
    else
        std::cout << "[LOCKED] Note '" << getEticheta() << "' contains encrypted data.\n";
}

/**
 * @brief Updates the note content.
 */
void NotitaSecurizata::set_notita(const std::string& nouContinut)
{
    this->notita = nouContinut;
}

/**
 * @brief Returns the note content.
 */
const std::string& NotitaSecurizata::getNotita() const
{ return notita; }

/**
 * @brief Returns the note data for saving.
 */
std::map<std::string, std::string> NotitaSecurizata::getDatePentruSalvare() const
{
    std::map<std::string, std::string> date;
    date["eticheta"] = getEticheta();
    date["notita"] = notita;
    return date;
}
