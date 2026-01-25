#ifndef OOP_MANAGER_H
#define OOP_MANAGER_H

#include <vector>
#include "EroareAcces.h"
#include "EroareValidare.h"
#include "ObiectNegasit.h"

template <typename T>
class Manager
{
    std::vector<T> elemente;

public:
    void adauga(const T& element) {
        elemente.push_back(element);
    }

    T& getElem(size_t index) {
        if (index < elemente.size())
            return elemente[index];
        else
            throw EroareAcces(static_cast<int>(index), static_cast<int>(elemente.size()));
    }

    [[nodiscard]] size_t getDimensiune() const {
        return elemente.size();
    }

    void stergeElem(size_t index) {
        if (index >= elemente.size())
            throw EroareAcces(static_cast<int>(index), static_cast<int>(elemente.size()));
        elemente.erase(elemente.begin() + index);
    }

    template <typename Predicat>
    T& gaseste(Predicat p) {
        for (auto& elem : elemente) {
            if (p(elem)) return elem;
        }
        throw ObiectNegasit("Criteriu cautare");
    }
};

#endif //OOP_MANAGER_H