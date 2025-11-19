#include "Utilizator.h"
#include "DateAutentificare.h"
#include "ObiectNegasit.h"
#include "EroareValidare.h"
#include "EroareAcces.h"

Utilizator::Utilizator(const Utilizator& sursa) : nume{sursa.nume}, email{sursa.email}, config{sursa.config}
{
    for (const auto& item : sursa.seif)
    {
        this->seif.push_back(item->clone());
    }
}

void Utilizator::swap(Utilizator& other) noexcept
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
    // E cont? (validare)
    const DateAutentificare* cont = dynamic_cast<const DateAutentificare*>(itemNou.get());
    if (cont != nullptr)
    {
        // Validare parola
        if (cont->getParola().length() < config.getLungimeMinimaParola())
        {
            throw EroareValidare(
                cont->getPlatforma(),
                static_cast<int>(cont->getParola().length()),
                static_cast<int>(config.getLungimeMinimaParola())
            );
        }
    }

    // Inserare sortata
    std::string etichetaNoua = itemNou->getEticheta();
    size_t i = 0;
    while (i < seif.size() && seif[i]->getEticheta() < etichetaNoua)
        i++;

    seif.insert(seif.begin() + static_cast<std::ptrdiff_t>(i), std::move(itemNou));
}

void Utilizator::stergeObiect(const std::string& eticheta)
{
    for (auto it = seif.begin(); it != seif.end(); ++it)
    {
        if (it->get()->getEticheta() == eticheta)
        {
            std::cout << "[INFO] S-a sters obiectul: " << eticheta << "\n";
            seif.erase(it);
            return;
        }
    }
    throw ObiectNegasit("Nu s-a gasit obiectul cu eticheta '" + eticheta + "' pentru stergere.");
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

Seif* Utilizator::getObiectAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(seif.size()))
    {
        throw EroareAcces(index, static_cast<int>(seif.size()));
    }
    return seif[index].get();
}