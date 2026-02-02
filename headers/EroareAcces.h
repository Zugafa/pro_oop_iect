#ifndef OOP_EROAREACCES_H
#define OOP_EROAREACCES_H
#include "ScriptException.h"
#include <string>

/**
 * @class EroareAcces
 * @brief Exception thrown when an invalid index is accessed in the vault.
 */
class EroareAcces : public ScriptException
{
private:
    int indexCerut;       ///< The index that was requested
    int dimensiuneMaxima; ///< The total number of elements available

public:
    /**
     * @brief Constructor for EroareAcces.
     * @param index Requested index.
     * @param max Current size of the collection.
     */
    EroareAcces(int index, int max);

    /**
     * @brief Provides a detailed suggestion for fixing the access error.
     * @return Suggestion string.
     */
    [[nodiscard]] std::string getSugestie() const override;
};
#endif //OOP_EROAREACCES_H