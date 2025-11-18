#include "Utilizator.h"
#include "DateAutentificare.h"

Utilizator::Utilizator(const Utilizator& sursa) : nume{sursa.nume}, email{sursa.email}, config{sursa.config}
{
    for (const auto& item : sursa.seif)
    {
        this->seif.push_back(item->clone());
    }
}

void Utilizator::swap(Utilizator& other)
{
    std::swap(nume, other.nume);
    std::swap(email, other.email);
    std::swap(seif, other.seif);
    std::swap(config, other.config);
}

Utilizator& Utilizator::operator=(Utilizator sursa)
{
    swap(sursa);
    return *this;
}


void Utilizator::adaugaObiect(std::unique_ptr<Seif> itemNou)
{
    DateAutentificare* cont = dynamic_cast<DateAutentificare*>(itemNou.get()); //dynamic cast
    if (cont != nullptr)
    {
        if (cont->getParola().length() < config.getLungimeMinimaParola())
        {
            std::cout << "[EROARE ADĂUGARE] Cont respins (" << cont->getPlatforma()
                << "): Parola este prea scurtă.\n";

            return;
        }
    }
    std::string etichetaNoua = itemNou->getEticheta();

    int i = 0;

    while (i < seif.size() && seif[i]->getEticheta() < etichetaNoua)
        i++;

    seif.insert(seif.begin() + i, std::move(itemNou));
}

void Utilizator::stergeObiect(const std::string& platforma, const std::string& username)
{
    for (auto it = seif.begin(); it != seif.end(); ++it)
    {
        DateAutentificare* cont = dynamic_cast<DateAutentificare*>(it->get());

        if (cont != nullptr)
        {
            if (cont->getPlatforma() == platforma && cont->getUtilizator() == username)
            {
                std::cout << "[INFO] S-a sters contul: " << platforma << "\n";
                seif.erase(it);
                return;
            }
        }
    }
    std::cout << "[EROARE] Nu s-a gasit contul " << platforma << " pentru stergere.\n";
}

std::ostream& operator<<(std::ostream& out, const Utilizator& user)
{
    out << "\n======================================================\n";
    out << "UTILIZATOR: " << user.nume << " (" << user.email << ")\n";

    // Folosim .size() în loc de nrConturi/capacitate
    out << "  -> Numar obiecte in seif: " << user.seif.size() << "\n";
    out << "------------------------------------------------------\n";

    if (user.seif.empty())
    {
        out << "   -- Niciun obiect adaugat --\n";
    }
    else
    {
        int i = 1;
        // Iterăm prin vectorul de unique_ptr
        for (const auto& item : user.seif)
        {
            out << "[" << i++ << "] ";

            // Apelăm operator<< (care apelează virtual afiseaza())
            // pentru a afișa polimorfic (Cont, Adresa, Card...)
            out << *item;
        }
    }
    out << "======================================================\n";
    return out;
}