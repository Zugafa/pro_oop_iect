#ifndef OOP_EROAREACCES_H
#define OOP_EROAREACCES_H
#include "ScriptException.h"
#include <string>

class EroareAcces : public ScriptException
{
private:
    int indexCerut;
    int dimensiuneMaxima;

public:
    EroareAcces(int index, int max);

    [[nodiscard]] std::string getSugestie() const override;
};
#endif //OOP_EROAREACCES_H