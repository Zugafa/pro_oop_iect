#include <iostream>
#include "CardBancar.h"

/**
 * @brief Creates a deep copy of the CardBancar object.
 */
std::shared_ptr<Seif> CardBancar::clone() const
{
    return std::make_shared<CardBancar>(*this);
}

/**
 * @brief Returns the type label.
 */
std::string CardBancar::getTip() const
{
    return "Card Bancar";
}

/**
 * @brief Displays card information.
 */
void CardBancar::afiseaza(std::ostream& os) const
{
    os << "Bank Card: " << numar << std::endl;
}

/**
 * @brief Validates CVV length and prints security status.
 */
// cppcheck-suppress unusedFunction
void CardBancar::verificaSecuritate() const
{
    if (cvv.length() != 3)
    {
        std::cout << "[ALERT] Card '" << getEticheta() << "': CVV code seems invalid (" << cvv.length() <<
            " digits).\n";
    }
    else
    {
        std::cout << "[SECURE] Card '" << getEticheta() << "' has a valid security format.\n";
    }
}

/**
 * @brief Updates card number.
 */
void CardBancar::setNumar(const std::string& n)
{
    this->numar = n;
}

/**
 * @brief Updates expiration date.
 */
void CardBancar::setDataExp(const std::string& d)
{
    this->dataExpirare = d;
}

/**
 * @brief Updates CVV code.
 */
void CardBancar::setCVV(const std::string& c)
{
    this->cvv = c;
}

/**
 * @brief Updates cardholder's name.
 */
void CardBancar::setNumeDetinator(const std::string& nD)
{
    this->numeDetinator = nD;
}

/**
 * @brief Returns card number.
 */
const std::string& CardBancar::getNumar() const
{
    return numar;
}

/**
 * @brief Returns expiration date.
 */
const std::string& CardBancar::getDataExp() const
{
    return dataExpirare;
}

/**
 * @brief Returns CVV code.
 */
const std::string& CardBancar::getCVV() const
{
    return cvv;
}

/**
 * @brief Returns the cardholder's name.
 */
const std::string& CardBancar::getNumeDetinator() const
{
    return numeDetinator;
}

/**
 * @brief Returns data for saving.
 * @note Includes the cardholder name under the "numeDetinator" key.
 */
std::map<std::string, std::string> CardBancar::getDatePentruSalvare() const
{
    std::map<std::string, std::string> date;
    date["eticheta"] = getEticheta();
    date["numar"] = numar;
    date["dataExpirare"] = dataExpirare;
    date["cvv"] = cvv;
    date["numeDetinator"] = numeDetinator;
    return date;
}
