#ifndef OOP_EROAREVALIDARE_H
#define OOP_EROAREVALIDARE_H
#include "ScriptException.h"
#include <string>

class EroareValidare : public ScriptException
{
private:
    std::string numeCont;

    int lungimePrimita;
    int lungimeMinima;
    bool esteEroareContinut;

public:
    // Constructor pentru erori de lungime
    EroareValidare(const std::string& cont, int len, int min);

    // Constructor pentru erori de continut
    EroareValidare(const std::string& cont, const std::string& motiv);

    [[nodiscard]] std::string getSugestie() const override;
};
#endif