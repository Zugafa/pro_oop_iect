#include "GestionarParole.h"
#include <fstream>
#include <sstream>

/**
 * @brief Returns the singleton instance of GestionarParole.
 */
GestionarParole& GestionarParole::getInstance()
{
    static GestionarParole instance;
    return instance;
}

/**
 * @brief Performs user login.
 */
void GestionarParole::login(const std::string& numeUtilizator, const std::string& parola)
{
    Utilizator& user_gasit = utilizatori.gaseste([&](const Utilizator& user)
    {
        return numeUtilizator == user.getNume() ? 1 : 0;
    });
    if (user_gasit.verificaParola(parola) == true)
        user_curent = &user_gasit;
    else
        throw EroareValidare(numeUtilizator, "Incorrect password.");
}

/**
 * @brief Returns the current user.
 */
Utilizator* GestionarParole::getUserCrt() const
{
    return user_curent;
}

/**
 * @brief Adds an object to the current user's vault.
 */
void GestionarParole::adaugaObiectInSeif(const std::string& tip, const std::map<std::string, std::string>& date) const
{
    if (user_curent == nullptr) throw EroareValidare("", "User not logged in.");
    else user_curent->adaugaObiect(SeifFactory::getInstance().creeazaSeif(tip, date));
}

constexpr char CHEIE_SECRETA = 0x5A; // 01011010

/**
 * @brief Saves vault data to a binary file with simple XOR encryption.
 */
void GestionarParole::salveazaDatelePeDisc() const
{
    if (!user_curent) return;

    // Open in BINARY mode
    std::ofstream out("vault.dat", std::ios::binary);
    if (!out.is_open()) return;

    size_t total = user_curent->getNrObiecte();
    for(size_t i = 0; i < total; ++i) {
        const Seif* item = user_curent->getObiectAt(static_cast<int>(i));
        auto date = item->getDatePentruSalvare();

        // Build data string
        std::stringstream ss;
        ss << item->getTip() << "|";
        for(const auto& [key, val] : date) {
            ss << key << ":" << val << "|";
        }

        std::string linie = ss.str();

        // Encrypt content
        for(char &c : linie) c ^= CHEIE_SECRETA;

        // Write LENGTH followed by DATA
        int len = static_cast<int>(linie.size());
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(linie.c_str(), len);
    }
    out.close();
    std::cout << "[SYSTEM] Data saved binary in vault.dat\n";
}

/**
 * @brief Loads and decrypts vault data from disk.
 */
void GestionarParole::incarcaDateleDePeDisc() const
{
    if (!user_curent) return;

    std::ifstream in("vault.dat", std::ios::binary);
    if (!in.is_open()) {
        std::cout << "[SYSTEM] No previously saved data found.\n";
        return;
    }

    // Read until end of file
    while(true) {
        // 1. Read LENGTH of the next block
        int len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));

        // If reached EOF or read failed, stop
        if (in.eof() || in.gcount() != sizeof(len)) break;

        if (len <= 0 || len > 1000000) continue; // Anti-corruption protection

        // 2. Read encrypted data based on length
        std::string linie(len, '\0');
        in.read(&linie[0], len);

        // 3. Decrypt
        for(char &c : linie) c ^= CHEIE_SECRETA;

        // 4. Parsing
        std::stringstream ss(linie);
        std::string segment;
        std::vector<std::string> segmente;

        while(std::getline(ss, segment, '|')) {
            segmente.push_back(segment);
        }

        if (segmente.empty()) continue;

        std::string tipObiect = segmente[0];
        std::map<std::string, std::string> dateMap;

        for(size_t i = 1; i < segmente.size(); ++i) {
            size_t pos = segmente[i].find(':');
            if (pos != std::string::npos) {
                std::string key = segmente[i].substr(0, pos);
                std::string val = segmente[i].substr(pos + 1);
                dateMap[key] = val;
            }
        }

        try {
            if (tipObiect == "Date Autentificare") tipObiect = "DateAutentificare";
            if (tipObiect == "Card Bancar") tipObiect = "CardBancar";
            if (tipObiect == "Notita Securizata") tipObiect = "NotitaSecurizata";

            user_curent->adaugaObiect(SeifFactory::getInstance().creeazaSeif(tipObiect, dateMap));
        } catch (...) {
            std::cerr << "[ERR] Error creating object: " << tipObiect << "\n";
        }
    }
    in.close();
}

