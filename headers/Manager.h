#ifndef OOP_MANAGER_H
#define OOP_MANAGER_H

#include <vector>
#include "EroareAcces.h"
#include "EroareValidare.h"
#include "ObiectNegasit.h"

/**
 * @class Manager
 * @brief Generic template class for managing a collection of elements.
 * @tparam T The type of elements to be managed.
 */
template <typename T>
class Manager
{
    std::vector<T> elemente; ///< Internal collection of elements

public:
    /**
     * @brief Adds a new element to the collection.
     * @param element The element to add.
     */
    void adauga(const T& element) {
        elemente.push_back(element);
    }

    /**
     * @brief Retrieves an element by index.
     * @param index The index of the element.
     * @return Reference to the element.
     * @throws EroareAcces if the index is out of bounds.
     */
    // cppcheck-suppress unusedFunction
    T& getElem(size_t index) {
        if (index < elemente.size())
            return elemente[index];
        else
            throw EroareAcces(static_cast<int>(index), static_cast<int>(elemente.size()));
    }

    /**
     * @brief Returns the number of elements in the collection.
     * @return The size of the collection.
     */
    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getDimensiune() const {
        return elemente.size();
    }

    /**
     * @brief Removes an element by index.
     * @param index The index of the element to remove.
     * @throws EroareAcces if the index is out of bounds.
     */
    // cppcheck-suppress unusedFunction
    void stergeElem(size_t index) {
        if (index >= elemente.size())
            throw EroareAcces(static_cast<int>(index), static_cast<int>(elemente.size()));
        elemente.erase(elemente.begin() + index);
    }

    /**
     * @brief Finds an element matching a given predicate.
     * @tparam Predicat The type of the predicate.
     * @param p The predicate function or functor.
     * @return Reference to the found element.
     * @throws ObiectNegasit if no element matches the predicate.
     */
    template <typename Predicat>
    T& gaseste(Predicat p) {
        for (auto& elem : elemente) {
            if (p(elem)) return elem;
        }
        throw ObiectNegasit("Search Criteria");
    }
};

#endif //OOP_MANAGER_H