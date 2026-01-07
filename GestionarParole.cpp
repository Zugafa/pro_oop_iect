#include "GestionarParole.h"

GestionarParole& GestionarParole::getInstance()
{
    static GestionarParole instance;
    return instance;
}

void GestionarParole::login(const std::string& numeUtilizator, const std::string& parola)
{
    Utilizator& user_gasit = utilizatori.gaseste([&](const Utilizator& user)
    {
        return numeUtilizator == user.getNume() ? 1 : 0;
    });
    if (user_gasit.verificaParola(parola) == true)
        user_curent = &user_gasit;
    else
        throw EroareValidare(numeUtilizator, "Parola incorecta.");
}
