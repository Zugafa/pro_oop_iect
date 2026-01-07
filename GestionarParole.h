#ifndef OOP_GESTIONARPAROLE_H
#define OOP_GESTIONARPAROLE_H
#include "Manager.h"
#include "Utilizator.h"
#include <map>

#include "SeifFactory.h"

class GestionarParole
{
    Manager<Utilizator> utilizatori;
    Utilizator* user_curent;

    GestionarParole() : user_curent(nullptr)
    {
    }

    GestionarParole(const GestionarParole& other) = delete;

    GestionarParole& operator=(const GestionarParole& other) = delete;

public:
    static GestionarParole& getInstance();

    Manager<Utilizator>& getManager() { return utilizatori; }

    void login(const std::string& numeUtilizator, const std::string& parola);

    void logout() { user_curent = nullptr; }

    [[nodiscard]] Utilizator* getUserCrt() const
    {
        return user_curent;
    }

    void adaugaObiectInSeif(const std::string& tip, const std::map<std::string, std::string>& date)
    {
        if (user_curent == nullptr) throw EroareValidare("", "User nelogat");
        else user_curent->adaugaObiect(SeifFactory::getInstance().creeazaSeif(tip, date));
    }
};


#endif //OOP_GESTIONARPAROLE_H
