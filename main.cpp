#include <iostream>
#include <string>
#include <cctype>
#include <utility>      // Pentru std::swap (dacă am folosi copy-and-swap)
#include <exception>    // Pentru std::exception
#include <cassert>      // Pentru testarea cu assert()

// =============================================================
// CLASA Configuratie (Clasa #3)
// =============================================================
class Configuratie
{
    std::string cheieVigenere, caractereSpecialeValide;
    int lungimeMinimaParola;

public:
    // Constructor de initializare cu valori default
    Configuratie()
        : cheieVigenere{"ENCRYPTER"},
          caractereSpecialeValide{"!@#$%^&*"},
          lungimeMinimaParola{8}
    {
    }

    // Gettere const
    [[nodiscard]] const std::string& getCheieVigenere() const { return cheieVigenere; }
    [[nodiscard]] const std::string& getCaractereSpecialeValide() const { return caractereSpecialeValide; }
    [[nodiscard]] int getLungimeMinimaParola() const { return lungimeMinimaParola; }

    ~Configuratie() = default; // Regula lui Zero

    // operator<<
    friend std::ostream& operator<<(std::ostream& out, const Configuratie& config);
};

// Definitia operator<< pentru Configuratie
std::ostream& operator<<(std::ostream& out, const Configuratie& config)
{
    out << "Setari Configurare:\n";
    out << "  -> Cheie Vigenere: " << config.cheieVigenere << "\n";
    out << "  -> Lungime Minima Parola: " << config.lungimeMinimaParola << "\n";
    out << "  -> Caractere Speciale: " << config.caractereSpecialeValide << "\n";
    return out; // Corectat: returnam 'out'
}

// =============================================================
// CLASA DateAutentificare (Clasa #1)
// =============================================================
class DateAutentificare
{
private:
    std::string numePlatforma, numeUtilizator, parola;
    Configuratie config; // Compunere

public:
    // Constructor explicit de inițializare (cu compunere)
    explicit DateAutentificare(const std::string& nPlat, const std::string& nUtiliz, const std::string& pass,
                               const Configuratie& configExterna)
        : numePlatforma{nPlat},
          numeUtilizator{nUtiliz},
          parola{pass},
          config{configExterna} // Initializam membrul config
    {
    }

    // Regula celor Trei (generate de compilator sunt ok, dar le scriem pt claritate)
    DateAutentificare(const DateAutentificare& sursa) = default;
    DateAutentificare& operator=(const DateAutentificare& sursa) = default;
    ~DateAutentificare() = default;

    // Declaratia operator<<
    friend std::ostream& operator<<(std::ostream& out, const DateAutentificare& date);

    // Declaratii friend pentru operatorii de comparare (necesari pt assert)
    friend bool operator==(const DateAutentificare& lhs, const DateAutentificare& rhs);
    friend bool operator!=(const DateAutentificare& lhs, const DateAutentificare& rhs);

    // Getters const
    const std::string& getPlatforma() const { return numePlatforma; }
    const std::string& getUtilizator() const { return numeUtilizator; }
    const std::string& getParola() const { return parola; }
    // Getter pentru config (necesar pt adaugaCont)
    const Configuratie& getConfig() const { return config; }

    // --- Funcții Membre Publice Netriviale (3 funcții) ---

    // 1. Setter Parola cu Validare
    void setter_parola(const std::string& nouaParola)
    {
        // Folosim obiectul 'config'
        if (nouaParola.length() < config.getLungimeMinimaParola())
        {
            std::cout << "[INFO] Parola n-a fost setata (prea scurta).\n";
            return;
        }

        bool areLiteraMare = false;
        bool areCaracterSpecial = false;
        std::string caractereValide = config.getCaractereSpecialeValide();

        for (const auto& litera : nouaParola)
        {
            if (isupper(litera)) areLiteraMare = true;
            else if (caractereValide.find(litera) != std::string::npos) areCaracterSpecial = true;
            if (areLiteraMare && areCaracterSpecial)
                break;
        }

        if (areLiteraMare && areCaracterSpecial)
        {
            this->parola = nouaParola;
            std::cout << "[INFO] Parola a fost setata cu succes pentru " << this->numePlatforma << "!\n";
            return;
        }
        std::cout << "Parola trebuie sa contina o litera mare si un caracter special\n";
    }

    // 2. Criptare Vigenere
    void CriptareVigenere()
    {
        std::string Cheie = config.getCheieVigenere(); // Folosim config
        int pozCheie = 0, lenCheie = Cheie.length();
        for (auto& literaParola : parola)
        {
            if (!std::isalpha(literaParola)) continue;
            char baza = std::isupper(literaParola) ? 'A' : 'a';
            int valLitera = literaParola - baza;
            int valCheie = Cheie[pozCheie % lenCheie] - 'A';
            int valNouaLitera = (valLitera + valCheie) % 26;
            literaParola = valNouaLitera + baza;
            pozCheie++;
        }
    }

    // 3. Decriptare Vigenere
    void deCriptareVigenere()
    {
        std::string Cheie = config.getCheieVigenere(); // Folosim config
        int pozCheie = 0, lenCheie = Cheie.length();
        for (auto& literaParola : parola)
        {
            if (!std::isalpha(literaParola)) continue;
            char baza = std::isupper(literaParola) ? 'A' : 'a';
            int valLiteraCriptata = literaParola - baza;
            int valCheie = Cheie[pozCheie % lenCheie] - 'A';
            int valLiteraNoua = (valLiteraCriptata - valCheie + 26) % 26;
            literaParola = valLiteraNoua + baza;
            pozCheie++;
        }
    }
};

// Definiția operator<< pentru DateAutentificare
std::ostream& operator<<(std::ostream& out, const DateAutentificare& date)
{
    out << "  -> Platforma: " << date.numePlatforma << " | Utilizator: " << date.numeUtilizator <<
        " | Parola Curenta: [" << date.parola << "]\n";
    return out;
}

// Definiția operator== (necesar pt assert)
bool operator==(const DateAutentificare& lhs, const DateAutentificare& rhs)
{
    // Comparam atributele private
    return lhs.numePlatforma == rhs.numePlatforma &&
        lhs.numeUtilizator == rhs.numeUtilizator &&
        lhs.parola == rhs.parola;
    // Nu comparam 'config' intentionat, e un detaliu de implementare
}

// Definiția operator!= (necesar pt assert)
bool operator!=(const DateAutentificare& lhs, const DateAutentificare& rhs)
{
    return !(lhs == rhs);
}


// -------------------------------------------------------------
// CLASA Utilizator (Clasa #2, Compunere)
// -------------------------------------------------------------
class Utilizator
{
private:
    std::string nume, email;
    DateAutentificare** conturi; // Compunere
    int nrConturi, capacitate;
    Configuratie config; // Compunere
    void realocaMemorie()
    {
        if (nrConturi == capacitate)
        {
            capacitate *= 2;
            DateAutentificare** nouArray = new DateAutentificare*[capacitate];
            for (int k = 0; k < nrConturi; k++) { nouArray[k] = conturi[k]; }
            delete[] conturi;
            conturi = nouArray;
            std::cout << "[INFO] Capacitatea vectorului a fost marita la " << capacitate << "\n";
        }
    }

public:
    // Constructor de inițializare (cu compunere)
    explicit Utilizator(const std::string& n, const std::string& e, const Configuratie& configExterna)
        : nume{n}, email{e}, nrConturi{0}, capacitate{10}, config{configExterna}
    {
        conturi = new DateAutentificare*[capacitate];
    }

    // --- REGULA CELOR TREI (CRITICA) ---

    // Constructor de copiere (Deep Copy)
    Utilizator(const Utilizator& sursa)
        : nume{sursa.nume}, email{sursa.email}, nrConturi{sursa.nrConturi},
          capacitate{sursa.capacitate}, config{sursa.config}
    {
        conturi = new DateAutentificare*[capacitate];
        for (int i = 0; i < nrConturi; i++)
            conturi[i] = new DateAutentificare(*sursa.conturi[i]); // Deep copy
    }

    // Operator de atribuire (Deep Copy)
    Utilizator& operator=(const Utilizator& sursa)
    {
        if (this == &sursa) return *this;
        for (int i = 0; i < this->nrConturi; i++) delete this->conturi[i];
        delete[] this->conturi;

        nume = sursa.nume;
        email = sursa.email;
        nrConturi = sursa.nrConturi;
        capacitate = sursa.capacitate;
        config = sursa.config;

        conturi = new DateAutentificare*[capacitate];
        for (int i = 0; i < nrConturi; i++)
            conturi[i] = new DateAutentificare(*sursa.conturi[i]); // Deep copy
        return *this;
    }

    // Destructor (Curățare)
    ~Utilizator()
    {
        for (int i = 0; i < nrConturi; i++) delete conturi[i];
        delete[] conturi;
    }

    // Declaratia operator<<
    friend std::ostream& operator<<(std::ostream& out, const Utilizator& user);

    // Declaratii friend pentru operatorii de comparare (necesari pt assert)
    friend bool operator==(const Utilizator& lhs, const Utilizator& rhs);
    friend bool operator!=(const Utilizator& lhs, const Utilizator& rhs);

    // Getter securizat (const)
    DateAutentificare* getContByIndex(int index) const
    {
        if (index >= 0 && index < nrConturi) return conturi[index];
        return nullptr;
    }

    // --- Funcții Membre Publice Netriviale ---

    // 1. Adauga cont (cu realocare, validare si INSERARE SORTATA)
    void adaugaCont(DateAutentificare* nouCont)
    {
        // Validare: folosim getter-ul din DateAutentificare pt a accesa config-ul
        if (nouCont->getParola().length() < nouCont->getConfig().getLungimeMinimaParola())
        {
            std::cout << "[EROARE ADĂUGARE] Cont respins (" << nouCont->getPlatforma() <<
                "): Parola este prea scurtă.\n";
            delete nouCont; // Prevenire memory leak
            return;
        }

        realocaMemorie();

        std::string platformaNoua = nouCont->getPlatforma();
        int i = 0;
        while (i < nrConturi && conturi[i]->getPlatforma() < platformaNoua) { i++; }
        for (int j = nrConturi; j > i; j--) { conturi[j] = conturi[j - 1]; }

        conturi[i] = nouCont;
        nrConturi++;
    }

    // 2. Sterge cont (cu cautare si shift-left)
    void stergeCont(const std::string& platforma, const std::string& username)
    {
        for (int i = 0; i < nrConturi; i++)
        {
            if (conturi[i]->getPlatforma() == platforma && conturi[i]->getUtilizator() == username)
            {
                std::cout << "[INFO] S-a sters contul: " << platforma << "\n";
                delete conturi[i];
                for (int j = i; j < nrConturi - 1; j++) conturi[j] = conturi[j + 1];
                nrConturi--;
                return;
            }
        }
        std::cout << "[EROARE] Nu s-a gasit contul " << platforma << " pentru stergere.\n";
    }
};

// Definiția operator<< pentru Utilizator
std::ostream& operator<<(std::ostream& out, const Utilizator& user)
{
    out << "\n======================================================\n";
    out << "UTILIZATOR: " << user.nume << " (" << user.email << ")\n";
    out << "  -> Conturi: " << user.nrConturi << " / Capacitate: " << user.capacitate << "\n";
    out << "------------------------------------------------------\n";
    if (user.nrConturi == 0) { out << "   -- Niciun cont adaugat --\n"; }
    else
    {
        for (int i = 0; i < user.nrConturi; i++)
        {
            out << "[" << i + 1 << "]" << *user.conturi[i];
        }
    }
    out << "======================================================\n";
    return out;
}

// Definiția operator== (necesar pt assert)
bool operator==(const Utilizator& lhs, const Utilizator& rhs)
{
    // Comparam datele simple
    if (lhs.nume != rhs.nume || lhs.email != rhs.email || lhs.nrConturi != rhs.nrConturi)
        return false;

    // Comparam in profunzime (deep compare) fiecare cont
    for (int i = 0; i < lhs.nrConturi; i++)
    {
        // Folosim operator== de la DateAutentificare
        if (*(lhs.conturi[i]) != *(rhs.conturi[i]))
            return false;
    }

    return true; // Daca totul e identic
}

// Definiția operator!= (necesar pt assert)
bool operator!=(const Utilizator& lhs, const Utilizator& rhs)
{
    return !(lhs == rhs);
}


// =============================================================
// FUNCTIA MAIN (Scenariul de Testare)
// =============================================================
int main()
{
    std::cout << std::unitbuf;
    std::cout << "--- START TESTARE S.C.R.I.P.T. (PARTEA 1) ---\n";

    // 1. TEST CLASA Configuratie
    Configuratie configDefault;
    std::cout << configDefault;

    // 2. TEST CREARE UTILIZATOR
    Utilizator userPrincipal("Ionut Popescu", "ionut.popescu@gmail.com", configDefault);

    // 3. TEST adaugaCont (Validare, Realocare, Sortare)
    std::cout << "\n--- TEST ADĂUGARE ȘI SORTARE ---\n";
    userPrincipal.adaugaCont(new DateAutentificare("GitHub", "ionut_dev", "P@sWd12345", configDefault));
    userPrincipal.adaugaCont(new DateAutentificare("LinkedIn", "ionut.popescu", "L1nk3dIn!2025", configDefault));
    userPrincipal.adaugaCont(new DateAutentificare("Amazon", "ionut.amazon", "Secure!PassA1", configDefault));

    // Teste parola invalida (ar trebui respinse)
    userPrincipal.adaugaCont(new DateAutentificare("Facebook", "ionut_fb", "f1nallY", configDefault)); // Respins
    userPrincipal.adaugaCont(new DateAutentificare("Twitter", "ionut_x", "weak", configDefault)); // Respins

    userPrincipal.adaugaCont(new DateAutentificare("Netflix", "ionut.netflix", "N3tfLix25!", configDefault));

    // Lista ar trebui sa aiba 4 conturi, sortate: Amazon, GitHub, LinkedIn, Netflix
    std::cout << userPrincipal;

    // 4. TEST FUNCTII NETRIVIALE (Criptare, Decriptare, Setter)
    std::cout << "\n--- TEST CRIPTARE/DECRIPTARE VIGENERE ---\n";
    DateAutentificare* contAmazon = userPrincipal.getContByIndex(0); // Amazon
    if (contAmazon)
    {
        std::cout << "Original:" << *contAmazon;
        contAmazon->CriptareVigenere();
        std::cout << "Criptat: " << *contAmazon;
        contAmazon->deCriptareVigenere();
        std::cout << "Decriptat:" << *contAmazon;

        // Testam setter_parola
        contAmazon->setter_parola("ParolaNouaValida123!");
        contAmazon->setter_parola("scurta"); // Test respingere
        std::cout << "Dupa setter:" << *contAmazon;
    }

    // 5. TEST REGULA CELOR TREI (cu assert, conform cerintei)
    std::cout << "\n--- TEST REGULA CELOR TREI (cu assert) ---\n";

    // Test Constructor Copiere (Deep Copy)
    Utilizator userCopie = userPrincipal; // Aici se apeleaza Constructorul de Copiere
    assert((std::cout << "CC: Atributele se copiaza corect\n", userPrincipal == userCopie));

    // Modificam copia (adaugam un cont nou)
    userCopie.adaugaCont(new DateAutentificare("Z-Test", "z-user", "ParolaValida123", configDefault));
    std::cout << "[INFO] S-a adaugat 'Z-Test' in copie.\n";
    assert((std::cout << "CC: Modificarea copiei nu modifica originalul (Deep Copy)\n", userPrincipal != userCopie));

    // Test Operator Atribuire (Deep Copy)
    Utilizator userAtribuit("Vasile", "v@test.com", configDefault); // Obiect separat
    userAtribuit = userPrincipal; // Aici se apelează operator=
    assert((std::cout << "OP=: Atributele se copiaza corect\n", userPrincipal == userAtribuit));

    // Modificam noua copie (stergem un cont)
    userAtribuit.stergeCont("Amazon", "ionut.amazon");
    assert(
        (std::cout << "OP=: Modificarea copiei nu modifica originalul (Deep Copy)\n", userPrincipal != userAtribuit));

    // 6. TEST STERGERE CONT (din original)
    std::cout << "\n--- TEST STERGERE CONT (din original) ---\n";
    userPrincipal.stergeCont("LinkedIn", "ionut.popescu"); // Stergem din mijloc
    userPrincipal.stergeCont("NonExistent", "user"); // Test stergere esuata

    std::cout << userPrincipal; // Ar trebui sa aiba 3 conturi: Amazon, GitHub, Netflix

    std::cout << "\n--- TESTARE PARTEA I FINALIZATA ---\n";
    // Toti destructorii (userPrincipal, userCopie, userAtribuit) sunt apelati automat aici
    return 0;
}
