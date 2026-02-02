#include "Seif.h"

int Seif::nrObiecteTotale = 0;

/**
 * @brief Constructor for Seif.
 * Increments the global object counter.
 */
Seif::Seif(const std::string& eticheta) : eticheta(eticheta)
{
    nrObiecteTotale++;
}

/**
 * @brief Destructor for Seif.
 * Decrements the global object counter.
 */
Seif::~Seif()
{
    nrObiecteTotale--;
}

/**
 * @brief Returns the total number of objects.
 */
// cppcheck-suppress unusedFunction
int Seif::getNumarObiecteTotale()
{
    return nrObiecteTotale;
}

/**
 * @brief Returns the current label.
 */
const std::string& Seif::getEticheta() const
{ return eticheta; }

/**
 * @brief Updates the label.
 */
void Seif::setEticheta(const std::string& nouaEticheta) {
    eticheta = nouaEticheta;
}
