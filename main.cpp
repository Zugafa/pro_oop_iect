#include <iostream>
#include <string>
#include <cctype>

class DateAutentificare
{
    std::string numePlatforma, numeUtilizator, parola;

public:
    //constructor explicit de init cu lista de parametri
    DateAutentificare(const std::string& nPlat, const std::string& nUtiliz, const std::string& pass)
        : numePlatforma{nPlat},
          numeUtilizator{nUtiliz},
          parola{pass}
    {
    }

    //constructor copiere
    DateAutentificare(const DateAutentificare& sursa) : numePlatforma{sursa.numePlatforma},
                                                        numeUtilizator{sursa.numeUtilizator},
                                                        parola{sursa.parola}
    {
    }

    //supraincarcarea op =
    DateAutentificare& operator=(const DateAutentificare& sursa)
    {
        //verificare pentru cazuri de tip ob=ob
        if (this == &sursa)
            return *this;

        this->numePlatforma = sursa.numePlatforma;
        this->numeUtilizator = sursa.numeUtilizator;
        this->parola = sursa.parola;
        return *this;
    }

    //operatorul de scriere
    friend std::ostream& operator<<(std::ostream&, const DateAutentificare&);

    const std::string& getPlatforma() const
    {
        return numePlatforma;
    }

    const std::string& getUtilizator() const
    {
        return numeUtilizator;
    }

    const std::string& getParola() const
    {
        return parola;
    }

    //destructor default
    ~DateAutentificare() = default;

    //functie afisare
    void afisare() const
    {
        std::cout << "Nume platforma: " << numePlatforma
            << "\tUtilizator: " << numeUtilizator <<
            "\tParola: [hashed]\n";
    }

    //functie de setat o (alta) parola pentru o platforma
    void setter_parola(const std::string& nouaParola)
    {
        if (nouaParola.length() < 8)
        {
            std::cout << "Parola trebuie sa fie mai mare de 8 caractere\n";
            return;
        }

        bool areLiteraMare = false;
        bool areCaracterSpecial = false;
        std::string caractereSpeciale = "!@#$%^&*";

        for (const auto& litera : nouaParola)
        {
            if (isupper(litera))
                areLiteraMare = true;
            else if (caractereSpeciale.find(litera) != std::string::npos)
                areCaracterSpecial = true;
            if (areLiteraMare && areCaracterSpecial)
                break;
        }
        if (areLiteraMare && areCaracterSpecial)
        {
            parola = nouaParola;
            std::cout << "Parola a fost setata cu succes\n";
            return;
        }

        std::cout << "Parola trebuie sa contina o litera mare si un caracter special\n";
        return;
    }

    void CriptareVigenere()
    {
        std::string Cheie = "ENCRYPTER"; //daca se schimba cheia, rog sa se scrie CU CAPSLOCK
        int pozCheie = 0, lenCheie = Cheie.length();
        for (auto& literaParola : parola)
        {
            if (!isalpha(literaParola))
                continue;

            char baza = isupper(literaParola) ? 'A' : 'a';

            int valLitera = literaParola - baza;
            int valCheie = Cheie[pozCheie % lenCheie] - 'A';
            int valNouaLitera = (valLitera + valCheie) % 26;

            literaParola = valNouaLitera + baza;
            pozCheie++;
        }
    }

    void deCriptareVigenere()
    {
        std::string Cheie = "ENCRYPTER";
        int pozCheie = 0, lenCheie = Cheie.length();

        for (auto& literaParola : parola)
        {
            if (!isalpha(literaParola))
                continue;

            char baza = isupper(literaParola) ? 'A' : 'a';

            int valLiteraCriptata = literaParola - baza;
            int valCheie = Cheie[pozCheie % lenCheie] - 'A';
            int valLiteraNoua = (valLiteraCriptata - valCheie + 26) % 26;
            literaParola = valLiteraNoua + baza;
            pozCheie++;
        }
    }
};

std::ostream& operator<<(std::ostream& out, const DateAutentificare& date)
{
    out << "Nume platforma: " << date.numePlatforma
        << "\tUtilizator: " << date.numeUtilizator <<
        "\tParola: " << date.parola << std::endl;
    return out;
}

class Utilizator
{
    std::string nume, email;
    DateAutentificare** conturi;
    int nrConturi, capacitate;

public:
    //constructor de initializare
    Utilizator(const std::string& nume, const std::string& email)
        : nume{nume}, email{email}, nrConturi{0}, capacitate{10}
    {
        conturi = new DateAutentificare*[capacitate];
    }

    //constructor copiere
    Utilizator(const Utilizator& sursa)
        : nume{sursa.nume}, email{sursa.email}, nrConturi{sursa.nrConturi}, capacitate{sursa.capacitate}
    {
        conturi = new DateAutentificare*[capacitate];
        for (int i = 0; i < nrConturi; i++)
            conturi[i] = new DateAutentificare(*sursa.conturi[i]);
    }

    Utilizator& operator=(const Utilizator& sursa)
    {
        if (this == &sursa)
            return *this;

        for (int i = 0; i < this->nrConturi; i++)
            delete this->conturi[i]; // Șterge obiectele DateAutentificare vechi
        delete[] this->conturi;

        nume = sursa.nume;
        email = sursa.email;
        nrConturi = sursa.nrConturi;
        capacitate = sursa.capacitate;

        conturi = new DateAutentificare*[capacitate];
        for (int i = 0; i < nrConturi; i++)
            conturi[i] = new DateAutentificare(*sursa.conturi[i]);
        return *this;
    }

    friend std::ostream& operator<<(std::ostream&, const Utilizator&);

    DateAutentificare* getContByIndex(int index) const
    {
        if (index >= 0 && index < nrConturi)
            return conturi[index];
        return nullptr;
    }

    //destructor
    ~Utilizator()
    {
        for (int i = 0; i < nrConturi; i++)
            delete conturi[i];
        delete[] conturi;
    }

    void adaugaCont(DateAutentificare* nouCont)
    {
        if (nouCont->getParola().length() < 8)
        {
            std::cout << "[EROARE ADĂUGARE] Cont respins: Parola este prea scurtă (<8 caractere).\n";
            delete nouCont;
            return;
        }
        if (nrConturi == capacitate)
        {
            capacitate *= 2;
            DateAutentificare** nouArray = new DateAutentificare*[capacitate];
            for (int k = 0; k < nrConturi; k++)
                nouArray[k] = conturi[k];
            delete[] conturi;
            conturi = nouArray;
        }

        //inserare alfabetica
        std::string platformaNoua = nouCont->getPlatforma();

        int i = 0;
        while (i < nrConturi && conturi[i]->getPlatforma() < platformaNoua)
            i++;
        for (int j = nrConturi; j > i; j--)
            conturi[j] = conturi[j - 1];

        conturi[i] = nouCont;
        nrConturi++;
    }

    void stergeCont(const std::string& platforma, const std::string& username)
    {
        for (int i = 0; i < nrConturi; i++)
        {
            if (conturi[i]->getPlatforma() == platforma &&
                conturi[i]->getUtilizator() == username)
            {
                delete conturi[i];
                for (int j = i; j < nrConturi - 1; j++)
                    conturi[j] = conturi[j + 1];
                nrConturi--;
                return;
            }
        }
    }
};

std::ostream& operator<<(std::ostream& out, const Utilizator& user)
{
    out << user.nume << " " << user.email << "\n";
    out << "Conturi:\n";
    for (int i = 0; i < user.nrConturi; i++)
        out << *user.conturi[i] << "\n"; //se foloseste afisarea implementata deja la
    //DateAutentificare prin dereferentierea ob cu * ;)
    return out;
}


int main()
{
    std::cout << std::unitbuf;

    std::cout << "--- START TESTARE S.C.R.I.P.T. (PARTEA 1) ---\n";

    // 1. CREAREA UNUI UTILIZATOR (Testează constructorul Utilizator)
    Utilizator userPrincipal("Ionut Popescu", "ionut.popescu@gmail.com");
    std::cout << userPrincipal; // Testează operator<< pentru Utilizator

    // 2. ADĂUGAREA DE CONTURI ȘI TESTAREA INSERĂRII SORTATE (adaugaCont)
    std::cout << "\n--- TEST ADĂUGARE ȘI SORTARE (dupa Platforma) ---\n";

    // Adaugăm in ordine aleatorie pentru a verifica sortarea:
    userPrincipal.adaugaCont(new DateAutentificare("GitHub", "ionut_dev", "P@sWd12345"));
    userPrincipal.adaugaCont(new DateAutentificare("LinkedIn", "ionut.popescu", "L1nk3dIn!2025"));
    userPrincipal.adaugaCont(new DateAutentificare("Amazon", "ionut.amazon", "Secure!PassA1"));
    userPrincipal.adaugaCont(new DateAutentificare("Facebook", "ionut_fb", "f1nallY"));
    // Parola invalidă (mai putin de 8 caractere)

    // Cont invalid (va testa setter_parola)
    userPrincipal.adaugaCont(new DateAutentificare("Twitter", "ionut_x", "weak"));

    // Re-adaugam un cont cu parola valida
    userPrincipal.adaugaCont(new DateAutentificare("Netflix", "ionut.netflix", "N3tfLix25!"));

    // Afisăm. Ar trebui să vedem ordinea: Amazon, Facebook, GitHub, LinkedIn, Netflix, Twitter
    std::cout << userPrincipal;

    // 3. TESTAREA CRIPTĂRII / DECRIPTĂRII (funcții DateAutentificare)
    std::cout << "\n--- TEST CRIPTARE/DECRIPTARE VIGENERE ---\n";

    // Presupunem ca Amazon este la indexul 0 (datorita sortarii)
    DateAutentificare* contAmazon = userPrincipal.getContByIndex(0);

    std::cout << "Parola originala: " << *contAmazon;

    contAmazon->CriptareVigenere();
    std::cout << "Parola criptata: " << *contAmazon; // Testeaza operator<< cu parola schimbata

    contAmazon->deCriptareVigenere();
    std::cout << "Parola decriptata: " << *contAmazon;

    DateAutentificare* contLinkedIn = userPrincipal.getContByIndex(2);

    std::cout << "Parola originala: " << *contLinkedIn;

    contLinkedIn->CriptareVigenere();
    std::cout << "Parola criptata: " << *contLinkedIn; // Testeaza operator<< cu parola schimbata

    contLinkedIn->deCriptareVigenere();
    std::cout << "Parola decriptata: " << *contLinkedIn;

    // 4. TESTAREA COPIERII (Regula celor Trei: Constructor de Copiere)
    std::cout << "\n--- TEST COPIERE (Constructor de Copiere - Deep Copy) ---\n";

    Utilizator userCopie = userPrincipal; // Aici se apeleaza Constructorul de Copiere
    std::cout << "User Original (ptr u1): " << &userPrincipal << "\n";
    std::cout << "User Copie (ptr u2): " << &userCopie << "\n";

    // Modificăm contul Amazon in userCopie (ar trebui sa nu afecteze userPrincipal)
    userCopie.getContByIndex(0)->setter_parola("NEW_STRONG_PASS!1");
    std::cout << userCopie;
    std::cout << userPrincipal; // Contul original ar trebui sa aiba parola veche

    // 5. TESTAREA ATRIBUIRII (Regula celor Trei: operator=)
    std::cout << "\n--- TEST ATRIBUIRE (operator=) --- \n";
    Utilizator user3("Vasile", "v@example.com");
    user3 = userPrincipal; // Aici se apelează operator=

    // Testăm ștergerea dintr-o copie (user3). Daca userPrincipal nu crapa, e Deep Copy.
    user3.stergeCont("Netflix", "ionut.netflix");

    std::cout << user3;
    std::cout << userPrincipal; // Ar trebui sa fie inca 5 conturi, inclusiv Netflix

    // 6. TESTAREA ȘTERGERII FINALE (stergeCont)
    std::cout << "\n--- TEST ȘTERGERE CONT (Sterge Facebook si GitHub) ---\n";
    userPrincipal.stergeCont("Facebook", "ionut_fb"); // Sters in mijlocul listei
    userPrincipal.stergeCont("GitHub", "ionut_dev"); // Sters altundeva

    std::cout << userPrincipal;

    std::cout << "--- TESTARE FINALIZATA. Memoria va fi eliberata de Destructori (implicit, la iesirea din main) ---";

    // Destructorii vor fi apelati automat pentru userPrincipal, userCopie, si user3
    return 0;
}
