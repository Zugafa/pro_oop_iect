#include <iostream>
#include <string>
#include <memory>
#include <vector>

// --- INCLUDE-URI CLASE ---
#include "Utilizator.h"
#include "Configuratie.h"
#include "Seif.h"
#include "DateAutentificare.h"
#include "Identitate.h"
#include "CardBancar.h"
#include "NotitaSecurizata.h" // Dacă ai implementat-o

// --- INCLUDE-URI EXCEPȚII ---
#include "ScriptException.h"
#include "EroareValidare.h"
#include "ObiectNegasit.h"
#include "EroareAcces.h"

int main()
{
    // 1. Dezactivăm buffering-ul pentru afișare imediată
    std::cout << std::unitbuf;

    std::cout << "==================================================\n";
    std::cout << "   S.C.R.I.P.T. - SCENARIU TESTARE MILESTONE 2    \n";
    std::cout << "==================================================\n\n";

    // 0. TESTARE STATIC (La început, ar trebui să fie 0)
    std::cout << "[STATIC] Numar obiecte in memorie (Start): "
        << Seif::getNumarObiecteTotale() << "\n\n";

    // 2. Configurare inițială
    Configuratie configDefault;
    Utilizator user("Andrei Popescu", "andrei@email.com", configDefault);

    // -----------------------------------------------------------
    // TEST 1: ADĂUGARE POLIMORFĂ, SORTARE & STATIC
    // -----------------------------------------------------------
    std::cout << "--- [1] Testare Adaugare Obiecte & Polimorfism ---\n";
    try
    {
        // Adăugăm un Cont (DateAutentificare)
        // Nota: "GitHub" ar trebui să fie la mijloc după sortare
        user.adaugaObiect(std::make_unique<DateAutentificare>(
            "GitHub", "andrei_dev", "ParolaSigura123!", configDefault
        ));

        // Adăugăm o Identitate (Clasă nouă derivată din Seif)
        // "Acasa" ar trebui să fie prima (alfabetic)
        user.adaugaObiect(std::make_unique<Identitate>(
            "Acasa", "Andrei", "Popescu", "0722123123", "andrei@email.com",
            "Bld. Unirii", "Bucuresti", "Bucuresti", "Romania", "030123"
        ));

        // Adăugăm un Card Bancar (Clasă nouă derivată din Seif)
        // "Revolut" ar trebui să fie ultima
        user.adaugaObiect(std::make_unique<CardBancar>(
            "Revolut", "5354-1234-5678-9000", "10/28", "456"
        ));

        std::cout << "[OK] 3 Obiecte adaugate cu succes.\n";

        // Verificăm variabila statică (ar trebui să fie 3)
        std::cout << "[STATIC] Numar obiecte in memorie (Dupa adaugare): "
            << Seif::getNumarObiecteTotale() << "\n";

        // Afișăm seiful (demonstrează operator<< polimorfic)
        std::cout << user;
    }
    catch (const ScriptException& e)
    {
        std::cout << "[CRITIC] Eroare neasteptata: " << e.what() << "\n";
    }

    // -----------------------------------------------------------
    // TEST 2: EXCEPȚII "SMART" (EroareValidare)
    // -----------------------------------------------------------
    std::cout << "\n--- [2] Testare Validare Parola (Exceptie) ---\n";
    try
    {
        std::cout << "Incercam sa adaugam un cont cu parola slaba ('slaba')...\n";

        // Aceasta va arunca o excepție și obiectul NU va fi adăugat
        user.adaugaObiect(std::make_unique<DateAutentificare>(
            "Facebook", "andrei_fb", "slaba", configDefault // < 8 caractere
        ));
    }
    catch (const ScriptException& e)
    {
        // Prindem excepția prin referință la bază
        std::cout << "[PRINS!] " << e.what() << "\n";
        // Aici vedem "inteligența" excepției (calculul matematic)
        std::cout << "[SUGESTIE] " << e.getSugestie() << "\n";
    }

    // -----------------------------------------------------------
    // TEST 3: DYNAMIC_CAST CU SENS & FUNCȚII SPECIFICE TEMEI
    // -----------------------------------------------------------
    std::cout << "\n--- [3] Testare dynamic_cast & Criptare ---\n";
    try
    {
        // Luăm obiectul de la indexul 1 (GitHub, după sortarea Acasa, GitHub, Revolut)
        Seif* obiectGeneric = user.getObiectAt(1);

        std::cout << "Am selectat obiectul: " << obiectGeneric->getEticheta() << "\n";

        // Încercăm să facem downcast la DateAutentificare
        // Încercăm să facem downcast la DateAutentificare
        if (auto* cont = dynamic_cast<DateAutentificare*>(obiectGeneric))
        {
            std::cout << "Am gasit contul: " << cont->getEticheta() << "\n";

            // [FIX CPPCHECK] Folosim getter-ul neutilizat
            std::cout << "Utilizator asociat: " << cont->getUtilizator() << "\n";

            std::cout << "Parola in clar: " << cont->getParola() << "\n";

            // [FIX CPPCHECK] Folosim setter-ul neutilizat
            std::cout << "Testam schimbarea parolei...\n";
            cont->setter_parola("ParolaNouaForta123!");
            std::cout << "Parola noua: " << cont->getParola() << "\n";

            // [FIX CPPCHECK] Folosim getConfig (macar il apelam)
            // (void) spune compilatorului ca ignoram rezultatul, doar vrem sa apelam functia
            (void)cont->getConfig();

            // Testam criptarea pe noua parola
            cont->CriptareVigenere();
            std::cout << "Dupa criptare: " << cont->getParola() << "\n";

            cont->deCriptareVigenere();
            std::cout << "Dupa decriptare: " << cont->getParola() << "\n";
        }
        else
        {
            std::cout << "Obiectul nu este un cont de autentificare (nu se poate cripta).\n";
        }
    }
    catch (const ScriptException& e)
    {
        std::cout << "Eroare: " << e.what() << "\n";
    }

    // -----------------------------------------------------------
    // TEST 4: AUDIT SECURITATE (Funcție Virtuală Specifică Temei)
    // -----------------------------------------------------------
    std::cout << "\n--- [4] Testare Audit Securitate (Polimorfism) ---\n";
    // Iterăm manual pentru a apela funcția specifică
    // Atenție: folosim try-catch pentru getObiectAt
    try
    {
        for (int i = 0; i < Seif::getNumarObiecteTotale(); ++i)
        {
            Seif* item = user.getObiectAt(i);
            // Polimorfismul va alege funcția corectă pentru Cont, Card sau Identitate
            item->verificaSecuritate();
        }
    }
    catch (...)
    {
    }

    // -----------------------------------------------------------
    // TEST 5: EXCEPȚII DE ACCES (EroareAcces)
    // -----------------------------------------------------------
    std::cout << "\n--- [5] Testare Index Invalid ---\n";
    try
    {
        // Încercăm să accesăm indexul 100 (când avem doar 3 elemente)
        (void)user.getObiectAt(100);
    }
    catch (const ScriptException& e)
    {
        std::cout << "[PRINS!] " << e.what() << "\n";
        std::cout << "[SUGESTIE] " << e.getSugestie() << "\n";
    }

    // -----------------------------------------------------------
    // TEST 6: ȘTERGERE & EXCEPȚII (ObiectNegasit)
    // -----------------------------------------------------------
    std::cout << "\n--- [6] Testare Stergere ---\n";
    try
    {
        // Ștergere reușită
        user.stergeObiect("Acasa");

        // Ștergere eșuată (obiectul nu mai există)
        user.stergeObiect("Acasa");
    }
    catch (const ScriptException& e)
    {
        std::cout << "[PRINS!] " << e.what() << "\n";
        std::cout << "[SUGESTIE] " << e.getSugestie() << "\n";
    }

    // Verificăm contorul static (ar trebui să fi scăzut cu 1)
    std::cout << "[STATIC] Numar obiecte in memorie (Dupa stergere): "
        << Seif::getNumarObiecteTotale() << "\n";

    // -----------------------------------------------------------
    // TEST 7: DEEP COPY (Regula celor 3 via Smart Pointers)
    // -----------------------------------------------------------
    std::cout << "\n--- [7] Testare Deep Copy (Copy Constructor) ---\n";
    {
        // Creăm o clonă a utilizatorului
        Utilizator userClona = user;

        std::cout << "Am creat 'userClona'. Stergem tot din ea...\n";
        // Ștergem tot din clonă (GitHub și Revolut)
        userClona.stergeObiect("GitHub");
        userClona.stergeObiect("Revolut");

        std::cout << "Continut Clona (Ar trebui sa fie GOL): " << userClona << "\n";
        std::cout << "Continut Original (Trebuie sa aiba inca 2 obiecte): " << user << "\n";

        // Aici userClona este distrus (iese din scop).
        // Destructorul său nu trebuie să afecteze user-ul original.
    }

    std::cout << "\n=== FINAL DEMO ===\n";

    // La final, destructorul lui 'user' va distruge obiectele rămase.
    // Variabila statică ar trebui să ajungă la 0 după return.
    return 0;
}