#include "SecurityMonitor.h"
#include "Utilizator.h"
#include "DateAutentificare.h"
#include "CardBancar.h"
#include "NotitaSecurizata.h"
#include "Identitate.h"
#include <iostream>

SecurityMonitor::SecurityMonitor(Utilizator& u) : subiect(u) {
}

void SecurityMonitor::update() {
    alerte.clear();

    // VARIANTĂ SIGURĂ: Iterăm exact cât trebuie, fără excepții
    size_t total = subiect.getNrObiecte();

    for (size_t i = 0; i < total; ++i) {
        // Obținem obiectul direct (știm sigur că indexul e valid)
        Seif* item = subiect.getObiectAt(static_cast<int>(i));

        if (!item) continue; // Protecție extra

        // 1. Verificare Parole (DateAutentificare)
        if (const auto* cont = dynamic_cast<DateAutentificare*>(item)) {
            // Verificăm lungimea parolei
            if (cont->getParola().length() < 12) {
                alerte.push_back("[WARN] Parola contului '" + cont->getEticheta() + "' este slabă (<12 caractere).");
            }
        }

        // 2. Verificare Carduri (CardBancar)
        else if (auto* card = dynamic_cast<CardBancar*>(item)) {
            // Exemplu: verificăm lungimea CVV-ului
            if (card->getCVV().length() != 3) {
                alerte.push_back("[CRITICAL] Cardul '" + card->getEticheta() + "' are un CVV invalid.");
            }
            // Verificare expirare (exemplu simplist)
            if (card->getDataExp().find("/24") != std::string::npos) {
                alerte.push_back("[INFO] Cardul '" + card->getEticheta() + "' expiră anul acesta.");
            }
        }

        // 3. Verificare Notițe (NotitaSecurizata)
        else if (auto* nota = dynamic_cast<NotitaSecurizata*>(item)) {
            if (nota->getNotita().empty()) {
                alerte.push_back("[INFO] Notița '" + nota->getEticheta() + "' este goală.");
            }
        }
    }
}