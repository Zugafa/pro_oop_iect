/**
 * @file DateAutentificare.cpp
 * @brief Defines the DateAutentificare methods.
 */
#include "DateAutentificare.h"
#include "EroareValidare.h"
#include <array>
#include <cstdint>
#include <cstring>

/**
 * @brief Builds an authentication entry with explicit fields.
 * @param eticheta_ Platform label.
 * @param utilizator_ Username.
 * @param parola_ Password.
 * @param url_ Platform URL.
 * @param note_ Additional notes.
 */
DateAutentificare::DateAutentificare(const std::string& eticheta_,
                                     const std::string& utilizator_,
                                     const std::string& parola_,
                                     const std::string& url_,
                                     const std::string& note_)
    : Seif(eticheta_),
      utilizator(utilizator_),
      parola(parola_),
      url(url_),
      note(note_)
{
    numePlatforma = eticheta_;
}

/**
 * @brief Builds an authentication entry from a key/value map.
 * @param date Map containing "platforma", "utilizator", "parola", "url", "note".
 * @throws std::out_of_range if any required key is missing.
 */
DateAutentificare::DateAutentificare(const std::map<std::string, std::string>& date)
    : Seif(date.at("platforma")),
      utilizator(date.at("utilizator")),
      parola(date.at("parola")),
      url(date.at("url")),
      note(date.at("note"))
{
    numePlatforma = date.at("platforma");
}

/**
 * @brief Sets the label and updates the platform name.
 * @param etichetaNoua New label/platform name.
 */
void DateAutentificare::setEticheta(const std::string& etichetaNoua)
{
    Seif::setEticheta(etichetaNoua);
    numePlatforma = etichetaNoua;
}

/**
 * @brief Validates and sets a new password.
 *
 * Checks for:
 * 1. Minimum length (from Configuratie).
 * 2. At least one uppercase letter.
 * 3. At least one special character.
 * @param nouaParola Candidate password.
 * @throws EroareValidare if the password does not meet security requirements.
 */
void DateAutentificare::setter_parola(const std::string& nouaParola)
{
    if (nouaParola.length() < Configuratie::getInstance().getLungimeMinimaParola())
    {
        throw EroareValidare(
            numePlatforma,
            static_cast<int>(nouaParola.length()),
            static_cast<int>(Configuratie::getInstance().getLungimeMinimaParola())
        );
    }

    bool areLiteraMare = false;
    bool areCaracterSpecial = false;
    std::string caractereValide = Configuratie::getInstance().getCaractereSpecialeValide();

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
        return;
    }
    throw EroareValidare(numePlatforma, "Missing mandatory characters (Uppercase/Symbol).");
}

/**
 * @brief Updates the username.
 * @param nouNume New username.
 */
void DateAutentificare::setter_numeUtilizator(const std::string& nouNume)
{
    this->utilizator = nouNume;
}

/**
 * @brief Updates the URL.
 * @param url_ New URL.
 */
void DateAutentificare::setter_url(const std::string& url_) {
    url = url_;
}

/**
 * @brief Updates the notes.
 * @param note_ New notes.
 */
void DateAutentificare::setter_note(const std::string& note_) {
    note = note_;
}

/**
 * @brief SHA3-based stream cipher to protect the stored password.
 *
 * Uses a SHA3-256 derived keystream (keyed by the configuration secret) to XOR
 * the password bytes. Applying it twice (encrypt/decrypt) restores the clear text.
 */
void DateAutentificare::CriptareVigenere()
{
    auto key = Configuratie::getInstance().getCheieVigenere();
    if (key.empty() || parola.empty()) return;

    // --- tiny SHA3-256 (keccak) implementation (public domain style) ---
    auto sha3_256 = [](const uint8_t* in, size_t inlen)
    {
        constexpr size_t hashlen = 32;
        constexpr size_t rate = 136; // 1088 bits
        constexpr uint64_t RC[24] = {
            0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
            0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
            0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
            0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
            0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL, 0x800000008000000aULL,
            0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
        };

        auto rol64 = [](uint64_t x, int s) { return (x << s) | (x >> (64 - s)); };

        uint64_t st[25] = {0};
        size_t offset = 0;
        while (inlen >= rate)
        {
            for (size_t i = 0; i < rate / 8; ++i)
            {
                uint64_t v;
                std::memcpy(&v, in + offset + i * 8, 8);
                st[i] ^= v;
            }
            // keccak-f
            for (int round = 0; round < 24; ++round)
            {
                uint64_t C[5], D[5];
                for (int x = 0; x < 5; ++x) C[x] = st[x] ^ st[x + 5] ^ st[x + 10] ^ st[x + 15] ^ st[x + 20];
                for (int x = 0; x < 5; ++x) D[x] = rol64(C[(x + 1) % 5], 1) ^ C[(x + 4) % 5];
                for (int x = 0; x < 5; ++x) for (int y = 0; y < 5; ++y) st[x + 5 * y] ^= D[x];

                uint64_t B[25];
                B[0] = st[0];
                B[1] = rol64(st[6], 44);   B[2] = rol64(st[12], 43);  B[3] = rol64(st[18], 21);  B[4] = rol64(st[24], 14);
                B[5] = rol64(st[3], 28);   B[6] = rol64(st[9], 20);   B[7] = rol64(st[10], 3);   B[8] = rol64(st[16], 45);  B[9] = rol64(st[22], 61);
                B[10] = rol64(st[1], 1);   B[11] = rol64(st[7], 6);   B[12] = rol64(st[13], 25); B[13] = rol64(st[19], 8);  B[14] = rol64(st[20], 18);
                B[15] = rol64(st[4], 27);  B[16] = rol64(st[5], 36);  B[17] = rol64(st[11], 10); B[18] = rol64(st[17], 15); B[19] = rol64(st[23], 56);
                B[20] = rol64(st[2], 62);  B[21] = rol64(st[8], 55);  B[22] = rol64(st[14], 39); B[23] = rol64(st[15], 41); B[24] = rol64(st[21], 2);

                for (int y = 0; y < 5; ++y)
                {
                    for (int x = 0; x < 5; ++x)
                    {
                        st[x + 5 * y] = B[x + 5 * y] ^ ((~B[((x + 1) % 5) + 5 * y]) & B[((x + 2) % 5) + 5 * y]);
                    }
                }
                st[0] ^= RC[round];
            }
            offset += rate;
            inlen -= rate;
        }

        // padding
        std::array<uint8_t, rate> block{};
        if (inlen > 0) std::memcpy(block.data(), in + offset, inlen);
        block[inlen] = 0x06;
        block[rate - 1] |= 0x80;
        for (size_t i = 0; i < rate / 8; ++i)
        {
            uint64_t v;
            std::memcpy(&v, block.data() + i * 8, 8);
            st[i] ^= v;
        }
        // final keccak-f
        for (int round = 0; round < 24; ++round)
        {
            uint64_t C[5], D[5];
            for (int x = 0; x < 5; ++x) C[x] = st[x] ^ st[x + 5] ^ st[x + 10] ^ st[x + 15] ^ st[x + 20];
            for (int x = 0; x < 5; ++x) D[x] = rol64(C[(x + 1) % 5], 1) ^ C[(x + 4) % 5];
            for (int x = 0; x < 5; ++x) for (int y = 0; y < 5; ++y) st[x + 5 * y] ^= D[x];

            uint64_t B[25];
            B[0] = st[0];
            B[1] = rol64(st[6], 44);   B[2] = rol64(st[12], 43);  B[3] = rol64(st[18], 21);  B[4] = rol64(st[24], 14);
            B[5] = rol64(st[3], 28);   B[6] = rol64(st[9], 20);   B[7] = rol64(st[10], 3);   B[8] = rol64(st[16], 45);  B[9] = rol64(st[22], 61);
            B[10] = rol64(st[1], 1);   B[11] = rol64(st[7], 6);   B[12] = rol64(st[13], 25); B[13] = rol64(st[19], 8);  B[14] = rol64(st[20], 18);
            B[15] = rol64(st[4], 27);  B[16] = rol64(st[5], 36);  B[17] = rol64(st[11], 10); B[18] = rol64(st[17], 15); B[19] = rol64(st[23], 56);
            B[20] = rol64(st[2], 62);  B[21] = rol64(st[8], 55);  B[22] = rol64(st[14], 39); B[23] = rol64(st[15], 41); B[24] = rol64(st[21], 2);

            for (int y = 0; y < 5; ++y)
            {
                for (int x = 0; x < 5; ++x)
                {
                    st[x + 5 * y] = B[x + 5 * y] ^ ((~B[((x + 1) % 5) + 5 * y]) & B[((x + 2) % 5) + 5 * y]);
                }
            }
            st[0] ^= RC[round];
        }

        std::array<uint8_t, hashlen> out{};
        for (size_t i = 0; i < hashlen / 8; ++i) std::memcpy(out.data() + i * 8, &st[i], 8);
        return out;
    };

    // XOR the password with SHA3-derived keystream blocks
    std::string counterPrefix = key + ":";
    for (size_t offset = 0, block = 0; offset < parola.size(); ++block)
    {
        std::string counter = counterPrefix + std::to_string(block);
        auto digest = sha3_256(reinterpret_cast<const uint8_t*>(counter.data()), counter.size());
        size_t chunk = std::min<size_t>(digest.size(), parola.size() - offset);
        for (size_t i = 0; i < chunk; ++i)
        {
            parola[offset + i] = static_cast<char>(static_cast<uint8_t>(parola[offset + i]) ^ digest[i]);
        }
        offset += chunk;
    }
}

/**
 * @brief Decrypts the password using the same SHA3-based stream cipher.
 *
 * Applies the same keystream XOR to restore the clear password.
 */
void DateAutentificare::deCriptareVigenere()
{
    CriptareVigenere(); // symmetric XOR with SHA3-derived keystream
}

/**
 * @brief Checks the security level of the password and outputs a warning if needed.
 *
 * Emits a message to std::cout based on the password length.
 */
void DateAutentificare::verificaSecuritate() const
{
    if (parola.length() < 12)
        std::cout << "[WARN] Account " << getEticheta() << " has medium security.\n";
    else
        std::cout << "[OK] Account " << getEticheta() << " is secure.\n";
}

/**
 * @brief Returns all data fields in a map for persistence.
 * @return Map with field names as keys and field values as values.
 */
std::map<std::string, std::string> DateAutentificare::getDatePentruSalvare() const
{
    std::map<std::string, std::string> date;
    date["platforma"] = getEticheta();
    date["utilizator"] = utilizator;
    date["parola"] = parola;
    date["url"] = url;
    date["note"] = note;
    return date;
}
