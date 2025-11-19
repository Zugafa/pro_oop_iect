#ifndef OOP_OBIECTNEGASIT_H
#define OOP_OBIECTNEGASIT_H
#include "ScriptException.h"
#include <string>

class ObiectNegasit : public ScriptException
{
private:
    std::string etichetaCautata;

public:
    explicit ObiectNegasit(const std::string& eticheta);

    [[nodiscard]] std::string getSugestie() const override;
};
#endif