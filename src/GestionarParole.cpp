#include "GestionarParole.h"
#include <fstream>
#include <sstream>

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

Utilizator* GestionarParole::getUserCrt() const
{
    return user_curent;
}

void GestionarParole::adaugaObiectInSeif(const std::string& tip, const std::map<std::string, std::string>& date) const
{
    if (user_curent == nullptr) throw EroareValidare("", "User nelogat");
    else user_curent->adaugaObiect(SeifFactory::getInstance().creeazaSeif(tip, date));
}

constexpr char CHEIE_SECRETA = 0x5A; // 01011010

void GestionarParole::salveazaDatelePeDisc() const
{
    if (!user_curent) return;

    // Deschidem în mod BINAR
    std::ofstream out("vault.dat", std::ios::binary);
    if (!out.is_open()) return;

    size_t total = user_curent->getNrObiecte();
    for(size_t i = 0; i < total; ++i) {
        Seif* item = user_curent->getObiectAt(static_cast<int>(i));
        auto date = item->getDatePentruSalvare();

        // Construim șirul de date
        std::stringstream ss;
        ss << item->getTip() << "|";
        for(const auto& [key, val] : date) {
            ss << key << ":" << val << "|";
        }

        std::string linie = ss.str();

        // Criptăm conținutul
        for(char &c : linie) c ^= CHEIE_SECRETA;

        // --- FIX: Scriem LUNGIMEA (int) urmată de DATE ---
        int len = static_cast<int>(linie.size());
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(linie.c_str(), len);
    }
    out.close();
    std::cout << "[SYSTEM] Date salvate binar in vault.dat\n";
}

void GestionarParole::incarcaDateleDePeDisc() const
{
    if (!user_curent) return;

    std::ifstream in("vault.dat", std::ios::binary);
    if (!in.is_open()) {
        std::cout << "[SYSTEM] Nu exista date salvate anterior.\n";
        return;
    }

    // Citim până la finalul fișierului
    while(true) {
        // 1. Citim LUNGIMEA blocului următor
        int len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));

        // Dacă am ajuns la final (EOF) sau citirea a eșuat, ne oprim
        if (in.eof() || in.gcount() != sizeof(len)) break;

        if (len <= 0 || len > 1000000) continue; // Protecție anti-corupție

        // 2. Citim datele criptate pe baza lungimii
        std::string linie(len, '\0');
        in.read(&linie[0], len);

        // 3. Decriptare
        for(char &c : linie) c ^= CHEIE_SECRETA;

        // 4. Parsare (Logica veche funcționează acum perfect)
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
            std::cerr << "[ERR] Eroare la crearea obiectului: " << tipObiect << "\n";
        }
    }
    in.close();
}

