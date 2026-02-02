#include "Utilizator.h"
#include "DateAutentificare.h"
#include "ObiectNegasit.h"
#include "EroareValidare.h"
#include "EroareAcces.h"

/**
 * @brief Copy constructor performing deep copy of vault items.
 */
Utilizator::Utilizator(const Utilizator& sursa)
    : nume{sursa.nume},
      email{sursa.email},
      parolaMaster{sursa.parolaMaster}
{
    for (const auto& item : sursa.seif)
    {
        this->seif.push_back(item->clone());
    }
}

/**
 * @brief Swaps contents with another Utilizator.
 */
void Utilizator::swap(Utilizator& other) noexcept
{
    std::swap(nume, other.nume);
    std::swap(email, other.email);
    std::swap(seif, other.seif);
}

/**
 * @brief Assignment operator using copy-and-swap.
 */
Utilizator& Utilizator::operator=(Utilizator sursa)
{
    swap(sursa);
    return *this;
}

/**
 * @brief Verifies if the master password is correct.
 */
bool Utilizator::verificaParola(const std::string& parola) const
{
    return parola == parolaMaster;
}

/**
 * @brief Adds an item to the vault, ensuring sorted order by label.
 */
void Utilizator::adaugaObiect(std::shared_ptr<Seif> itemNou)
{
    // Is it an authentication entry? (validation)
    const auto* cont = dynamic_cast<const DateAutentificare*>(itemNou.get());
    if (cont != nullptr)
    {
        // Password validation
        const auto& config = Configuratie::getInstance();
        if (cont->getParola().length() < config.getLungimeMinimaParola())
        {
            throw EroareValidare(
                cont->getPlatforma(),
                static_cast<int>(cont->getParola().length()),
                static_cast<int>(config.getLungimeMinimaParola())
            );
        }
    }

    // Sorted insertion
    std::string etichetaNoua = itemNou->getEticheta();
    size_t i = 0;
    while (i < seif.size() && seif[i]->getEticheta() < etichetaNoua)
        i++;

    seif.insert(seif.begin() + static_cast<std::ptrdiff_t>(i), std::move(itemNou));
    istoricActiuni.adauga("Added: " + etichetaNoua);
    notify();
}

/**
 * @brief Removes an item from the vault by label.
 */
void Utilizator::stergeObiect(const std::string& eticheta)
{
    for (auto it = seif.begin(); it != seif.end(); ++it)
    {
        if (it->get()->getEticheta() == eticheta)
        {
            std::cout << "[INFO] Object deleted: " << eticheta << "\n";
            seif.erase(it);
            return;
        }
    }
    throw ObiectNegasit("Could not find object with label '" + eticheta + "' for deletion.");
}

/**
 * @brief Notifies observers of changes.
 */
void Utilizator::notify() const
{
    for (auto* obs : observatori) obs->update();
}

/**
 * @brief Formats user information and their vault contents for output.
 */
std::ostream& operator<<(std::ostream& out, const Utilizator& user)
{
    out << "\n======================================================\n";
    out << "USER: " << user.nume << " (" << user.email << ")\n";

    out << "  -> Number of items in vault: " << user.seif.size() << "\n";
    out << "------------------------------------------------------\n";

    if (user.seif.empty())
    {
        out << "   -- No items added --\n";
    }
    else
    {
        int i = 1;
        for (const auto& item : user.seif)
        {
            out << "[" << i++ << "] ";

            out << *item;
        }
    }
    out << "======================================================\n";
    return out;
}

/**
 * @brief Retrieves an item by index with bounds checking.
 */
Seif* Utilizator::getObiectAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(seif.size()))
    {
        throw EroareAcces(index, static_cast<int>(seif.size()));
    }
    return seif[index].get();
}

/**
 * @brief Returns the number of objects in the vault.
 */
size_t Utilizator::getNrObiecte() const
{
    return seif.size();
}

/**
 * @brief Attaches an observer.
 */
void Utilizator::attach(Observer* obs)
{
    observatori.push_back(obs);
}
