#ifndef OOP_SEIFFACTORY_H
#define OOP_SEIFFACTORY_H

#include <memory>
#include <string>
#include <map>
#include "Seif.h"

class SeifFactory
{
private:
    SeifFactory() = default;

    SeifFactory(const SeifFactory&) = delete;
    SeifFactory& operator=(const SeifFactory&) = delete;

public:
    static SeifFactory& getInstance();

    [[nodiscard]] std::shared_ptr<Seif> creeazaSeif(const std::string& tip,
                                                    const std::map<std::string, std::string>& date);
};

#endif
