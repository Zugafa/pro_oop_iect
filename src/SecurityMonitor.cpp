#include "SecurityMonitor.h"
#include "Utilizator.h"
#include "DateAutentificare.h"
#include "CardBancar.h"
#include "NotitaSecurizata.h"
#include "Identitate.h"
#include "Configuratie.h"
#include <iostream>
#include <unordered_map>
#include <regex>
#include <cctype>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <array>
#include <cstdio>
#include <vector>

/**
 * @brief Constructor for SecurityMonitor.
 */
SecurityMonitor::SecurityMonitor(Utilizator& u) : subiect(u), duplicateCount(0) {
}

void SecurityMonitor::addAlert(Severity sev, const std::string& msg)
{
    alerte.push_back(msg);
    alertDetails.push_back(SecurityAlert{sev, msg});
}

// Minimal SHA1 implementation
namespace
{
    using uint32 = uint32_t;

    std::array<uint8_t, 20> sha1(const std::string& data)
    {
        uint32 h0 = 0x67452301;
        uint32 h1 = 0xEFCDAB89;
        uint32 h2 = 0x98BADCFE;
        uint32 h3 = 0x10325476;
        uint32 h4 = 0xC3D2E1F0;

        std::vector<uint8_t> msg(data.begin(), data.end());
        size_t orig_len_bits = msg.size() * 8;
        msg.push_back(0x80);
        while ((msg.size() % 64) != 56) msg.push_back(0x00);
        for (int i = 7; i >= 0; --i) msg.push_back(static_cast<uint8_t>((orig_len_bits >> (i * 8)) & 0xFF));

        for (size_t chunk = 0; chunk < msg.size(); chunk += 64)
        {
            uint32 w[80];
            for (int i = 0; i < 16; ++i)
            {
                w[i] = (msg[chunk + i * 4] << 24) | (msg[chunk + i * 4 + 1] << 16) |
                    (msg[chunk + i * 4 + 2] << 8) | (msg[chunk + i * 4 + 3]);
            }
            for (int i = 16; i < 80; ++i)
            {
                uint32 val = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
                w[i] = (val << 1) | (val >> 31);
            }

            uint32 a = h0, b = h1, c = h2, d = h3, e = h4;

            for (int i = 0; i < 80; ++i)
            {
                uint32 f, k;
                if (i < 20) { f = (b & c) | ((~b) & d); k = 0x5A827999; }
                else if (i < 40) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
                else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
                else { f = b ^ c ^ d; k = 0xCA62C1D6; }

                uint32 temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
                e = d;
                d = c;
                c = (b << 30) | (b >> 2);
                b = a;
                a = temp;
            }

            h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
        }

        std::array<uint8_t, 20> out{};
        const uint32 hs[5] = {h0, h1, h2, h3, h4};
        for (int i = 0; i < 5; ++i)
        {
            out[i * 4 + 0] = static_cast<uint8_t>((hs[i] >> 24) & 0xFF);
            out[i * 4 + 1] = static_cast<uint8_t>((hs[i] >> 16) & 0xFF);
            out[i * 4 + 2] = static_cast<uint8_t>((hs[i] >> 8) & 0xFF);
            out[i * 4 + 3] = static_cast<uint8_t>((hs[i]) & 0xFF);
        }
        return out;
    }
}

SecurityMonitor::BreachInfo SecurityMonitor::checkPasswordOnline(const std::string& pwd) const
{
    BreachInfo info;
    info.checked = true;

    auto digest = sha1(pwd);
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');
    for (auto b : digest) oss << std::setw(2) << static_cast<int>(b);
    std::string fullHash = oss.str();
    std::string prefix = fullHash.substr(0, 5);
    std::string suffix = fullHash.substr(5);

    std::string cmd = "curl -s https://api.pwnedpasswords.com/range/" + prefix;
#ifdef _WIN32
    FILE* pipe = _popen(cmd.c_str(), "r");
#else
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe)
    {
        info.onlineAvailable = false;
        info.statusMsg = "Online breach check unavailable (curl failed).";
        return info;
    }

    std::string output;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) output += buffer;
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    if (output.empty())
    {
        info.onlineAvailable = false;
        info.statusMsg = "Online breach check unavailable (empty response).";
        return info;
    }

    std::istringstream in(output);
    std::string line;
    while (std::getline(in, line))
    {
        size_t pos = line.find(':');
        if (pos == std::string::npos) continue;
        std::string sfx = line.substr(0, pos);
        if (sfx == suffix)
        {
            info.onlineAvailable = true;
            info.pwned = true;
            info.count = std::stoi(line.substr(pos + 1));
            info.statusMsg = "Found in breaches (" + std::to_string(info.count) + " times).";
            return info;
        }
    }
    info.onlineAvailable = true;
    info.pwned = false;
    info.count = 0;
    info.statusMsg = "Password not found in online breach database.";
    return info;
}

/**
 * @brief Updates the list of security alerts by scanning the user's vault.
 */
void SecurityMonitor::update() {
    alerte.clear();
    alertDetails.clear();
    duplicateCount = 0;

    size_t total = subiect.getNrObiecte();

    // Pre-colectam item-urile pentru verificari incrucisate
    std::vector<DateAutentificare*> conturi;
    std::vector<CardBancar*> carduri;
    std::vector<NotitaSecurizata*> note;
    std::vector<Identitate*> identitati;

    for (size_t i = 0; i < total; ++i) {
        Seif* item = subiect.getObiectAt(static_cast<int>(i));

        if (!item) continue; 

        // 1. Password Verification (DateAutentificare)
        if (auto* cont = dynamic_cast<DateAutentificare*>(item)) conturi.push_back(cont);
        else if (auto* card = dynamic_cast<CardBancar*>(item)) carduri.push_back(card);
        else if (auto* nota = dynamic_cast<NotitaSecurizata*>(item)) note.push_back(nota);
        else if (auto* id = dynamic_cast<Identitate*>(item)) identitati.push_back(id);
    }

    // ---------- CONTURI ----------
    const auto& cfg = Configuratie::getInstance();
    std::unordered_map<std::string, std::vector<std::string>> paroleFolosite;

    for (const auto* cont : conturi)
    {
        const std::string pwd = cont->getParola();
        const std::string label = cont->getEticheta();
        paroleFolosite[pwd].push_back(label);

        if (pwd.length() < cfg.getLungimeMinimaParola())
            addAlert(Severity::Warning,
                     "[WARN] Password for '" + label + "' is below policy length.");

        if (pwd.length() < 12)
            addAlert(Severity::Warning,
                     "[WARN] Password for '" + label + "' is weak (<12 chars).");

        bool hasUpper = false, hasSymbol = false, hasDigit = false, hasLower = false;
        for (char c : pwd)
        {
            if (std::isupper(static_cast<unsigned char>(c))) hasUpper = true;
            if (std::islower(static_cast<unsigned char>(c))) hasLower = true;
            if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
            if (cfg.getCaractereSpecialeValide().find(c) != std::string::npos) hasSymbol = true;
        }
        if (!(hasUpper && hasSymbol))
            addAlert(Severity::Warning,
                     "[WARN] Password for '" + label + "' misses uppercase or symbol.");

        if ((hasDigit && !hasLower && !hasUpper && !hasSymbol) ||
            (!hasDigit && (hasLower ^ hasUpper) && !hasSymbol))
            addAlert(Severity::Warning,
                     "[WARN] Password for '" + label + "' uses limited character variety.");

        std::string loweredLabel = label;
        std::string loweredUser = cont->getUtilizator();
        std::transform(loweredLabel.begin(), loweredLabel.end(), loweredLabel.begin(), ::tolower);
        std::transform(loweredUser.begin(), loweredUser.end(), loweredUser.begin(), ::tolower);
        std::string loweredPwd = pwd;
        std::transform(loweredPwd.begin(), loweredPwd.end(), loweredPwd.begin(), ::tolower);

        if (loweredPwd == loweredUser || loweredPwd == loweredLabel)
            addAlert(Severity::Critical,
                     "[CRITICAL] Password for '" + label + "' matches the username/label.");
        // Online breach check
        BreachInfo bi = checkPasswordOnline(pwd);
        breachCache[label] = bi;
        if (bi.onlineAvailable && bi.pwned)
        {
            addAlert(Severity::Critical,
                     "[CRITICAL] Account '" + label + "' password found in breaches (" + std::to_string(bi.count) +
                     " hits).");
        }
        else if (!bi.onlineAvailable)
        {
            addAlert(Severity::Info,
                     "[INFO] Account '" + label + "': online breach check unavailable.");
        }
    }

    // Reutilizare parole
    for (const auto& entry : paroleFolosite)
    {
        if (entry.second.size() > 1)
        {
            std::string joined;
            for (size_t i = 0; i < entry.second.size(); ++i)
            {
                joined += entry.second[i];
                if (i + 1 < entry.second.size()) joined += ", ";
            }
            addAlert(Severity::Critical,
                     "[CRITICAL] Password reused across: " + joined);
            duplicateCount += static_cast<int>(entry.second.size());
        }
    }

    // Populate missing breach info entries for un-checked accounts (if any)
    for (const auto* cont : conturi)
    {
        const std::string label = cont->getEticheta();
        if (!breachCache.count(label))
        {
            breachCache[label] = BreachInfo{false, false, false, 0, "Not checked yet."};
        }
    }

    // ---------- CARDURI ----------
    auto luhnValid = [](const std::string& num) -> bool
    {
        int sum = 0;
        bool alt = false;
        for (auto it = num.rbegin(); it != num.rend(); ++it)
        {
            if (!std::isdigit(static_cast<unsigned char>(*it))) continue;
            int d = *it - '0';
            if (alt)
            {
                d *= 2;
                if (d > 9) d -= 9;
            }
            sum += d;
            alt = !alt;
        }
        return sum % 10 == 0;
    };

    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local{};
#ifdef _WIN32
    localtime_s(&local, &tt);
#else
    local = *std::localtime(&tt);
#endif
    int curYear = (local.tm_year + 1900) % 100;
    int curMonth = local.tm_mon + 1;

    for (const auto* card : carduri)
    {
        if (card->getCVV().length() != 3)
            addAlert(Severity::Critical,
                     "[CRITICAL] Card '" + card->getEticheta() + "' has an invalid CVV.");

        const std::string& holder = card->getNumeDetinator();
        if (holder.empty())
            addAlert(Severity::Warning,
                     "[WARN] Card '" + card->getEticheta() + "' has no cardholder name.");

        const std::string& exp = card->getDataExp();
        bool expParsed = false;
        int mm = 0, yy = 0;
        if (exp.size() == 5 && exp[2] == '/')
        {
            mm = std::stoi(exp.substr(0, 2));
            yy = std::stoi(exp.substr(3, 2));
            expParsed = (mm >= 1 && mm <= 12);
        }
        if (!expParsed)
        {
            addAlert(Severity::Warning,
                     "[WARN] Card '" + card->getEticheta() + "' has an invalid expiry format (MM/YY).");
        }
        else
        {
            bool expired = (yy < curYear) || (yy == curYear && mm < curMonth);
            int monthsDiff = (yy - curYear) * 12 + (mm - curMonth);
            if (expired)
                addAlert(Severity::Critical,
                         "[CRITICAL] Card '" + card->getEticheta() + "' is expired.");
            else if (monthsDiff <= 3)
                addAlert(Severity::Warning,
                         "[WARN] Card '" + card->getEticheta() + "' expires in under 3 months.");
        }

        std::string num = card->getNumar();
        std::string digits;
        for (char c : num) if (std::isdigit(static_cast<unsigned char>(c))) digits.push_back(c);
        if (digits.length() < 12 || digits.length() > 19)
            addAlert(Severity::Warning,
                     "[WARN] Card '" + card->getEticheta() + "' has an atypical number length.");
        else if (!luhnValid(digits))
            addAlert(Severity::Critical,
                     "[CRITICAL] Card '" + card->getEticheta() + "' fails Luhn validation.");
    }

    // ---------- NOTE ----------
    for (const auto* nota : note)
    {
        const std::string content = nota->getNotita();
        if (!content.empty() && content.size() < 8)
            addAlert(Severity::Info,
                     "[INFO] Note '" + nota->getEticheta() + "' is very short.");

        std::string lowered = content;
        std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
        if (lowered.find("password") != std::string::npos || lowered.find("parola") != std::string::npos ||
            lowered.find("pin") != std::string::npos)
        {
            addAlert(Severity::Warning,
                     "[WARN] Note '" + nota->getEticheta() + "' appears to contain sensitive keywords.");
        }
    }

    // ---------- IDENTITATI ----------
    std::regex emailRx(R"((\w|[\.-])+@(\w|[\.-]+)\.(\w+))");
    std::regex phoneRx(R"(^[0-9+][0-9\s\-]{6,}$)");

    for (const auto* id : identitati)
    {
        if (!std::regex_match(id->get_email(), emailRx))
            addAlert(Severity::Warning,
                     "[WARN] Identity '" + id->getEticheta() + "' has an invalid email.");
        if (!std::regex_match(id->get_telefon(), phoneRx))
            addAlert(Severity::Warning,
                     "[WARN] Identity '" + id->getEticheta() + "' has an invalid phone format.");
    }
}
SecurityMonitor::BreachInfo SecurityMonitor::getBreachInfo(const std::string& eticheta) const
{
    if (auto it = breachCache.find(eticheta); it != breachCache.end())
        return it->second;
    return BreachInfo{};
}
