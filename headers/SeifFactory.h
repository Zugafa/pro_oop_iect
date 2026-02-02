#ifndef OOP_SEIFFACTORY_H
#define OOP_SEIFFACTORY_H

#include <memory>
#include <string>
#include <map>
#include "Seif.h"

/**
 * @class SeifFactory
 * @brief Factory class for creating different types of vault items.
 * 
 * Implements the Singleton pattern and provides a centralized way to instantiate
 * subclasses of Seif based on a type string.
 */
class SeifFactory
{
private:
    SeifFactory() = default;
public:
    SeifFactory(const SeifFactory&) = delete;
    SeifFactory& operator=(const SeifFactory&) = delete;

    /**
     * @brief Returns the single instance of the SeifFactory class.
     * @return Reference to the singleton instance.
     */
    static SeifFactory& getInstance();

    /**
     * @brief Creates a vault item based on the specified type and data.
     * @param tip The type of item to create (e.g., "CardBancar", "DateAutentificare").
     * @param date A map of parameters for the constructor.
     * @return A shared pointer to the newly created Seif object.
     * @throws ObiectNegasit if the type is invalid.
     * @note For "CardBancar", the cardholder name can be provided as
     *       either "numeDetinator" or "holder".
     */
    [[nodiscard]] static std::shared_ptr<Seif> creeazaSeif(const std::string& tip,
                                                           const std::map<std::string, std::string>& date);
};

#endif
