#ifndef OOP_MANAGER_H
#define OOP_MANAGER_H
#include <vector>
#include <stdexcept>

template <typename T>
class Manager
{
    std::vector<T> elemente;

public:
    void adauga(const T& element)
    {
        elemente.push_back(element);
    }

    T& getElem(size_t index)
    {
        if (index < elemente.size())
            return elemente[index];
        else throw std::out_of_range("Index invalid");
    }

    size_t getDimensiune() const { return elemente.size(); }

    void stergeElem(size_t index)
    {
        if (index >= elemente.size()) throw std::out_of_range("Index invalid");
        elemente.erase(elemente.begin() + index);
    }
};


#endif //OOP_MANAGER_H
