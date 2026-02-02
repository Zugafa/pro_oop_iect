#include "PasswordGenerator.h"
#include <random>
#include <algorithm>
#include <cctype>

namespace
{
    std::string buildAlphabet(const PasswordGenerator::Options& opt)
    {
        const std::string lowerAll = "abcdefghijklmnopqrstuvwxyz";
        const std::string lowerEasy = "abcdefghijkmnopqrstuvwxyz"; // fara l
        const std::string upperAll = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const std::string upperEasy = "ABCDEFGHJKLMNPQRSTUVWXYZ"; // fara I O
        const std::string digitsAll = "0123456789";
        const std::string digitsEasy = "23456789"; // fara 0 1
        const std::string symbolsAll = "!@#$%^&*_-+=(){}[]~";
        const std::string symbolsEasy = "!@#$%-+";

        std::string alphabet;
        if (opt.useLower) alphabet += (opt.easyToType ? lowerEasy : lowerAll);
        if (opt.useUpper) alphabet += (opt.easyToType ? upperEasy : upperAll);
        if (opt.useDigits) alphabet += (opt.easyToType ? digitsEasy : digitsAll);
        if (opt.useSymbols) alphabet += (opt.easyToType ? symbolsEasy : symbolsAll);
        return alphabet;
    }
}

std::string PasswordGenerator::generate(const Options& opt)
{
    std::string alphabet = buildAlphabet(opt);
    if (alphabet.empty())
    {
        // Fallback to lowercase to avoid empty alphabet
        alphabet = "abcdefghijkmnopqrstuvwxyz";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(alphabet.size() - 1));

    std::string result;
    result.reserve(static_cast<size_t>(opt.length));
    for (int i = 0; i < opt.length; ++i)
    {
        result.push_back(alphabet[dist(gen)]);
    }
    return result;
}

PasswordGenerator::StrengthInfo PasswordGenerator::evaluateStrength(const std::string& pwd)
{
    StrengthInfo info;
    if (pwd.empty())
    {
        info.label = "Weak";
        info.score = 0.f;
        return info;
    }

    bool hasLower = false, hasUpper = false, hasDigit = false, hasSymbol = false;
    for (char c : pwd)
    {
        if (std::islower(static_cast<unsigned char>(c))) hasLower = true;
        else if (std::isupper(static_cast<unsigned char>(c))) hasUpper = true;
        else if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
        else hasSymbol = true;
    }

    int variety = static_cast<int>(hasLower) + static_cast<int>(hasUpper) +
        static_cast<int>(hasDigit) + static_cast<int>(hasSymbol);

    // Heuristic score (0..1)
    float lenScore = std::min(1.f, pwd.size() / 16.f);
    float varietyScore = static_cast<float>(variety) / 4.f;
    info.score = (lenScore * 0.6f) + (varietyScore * 0.4f);

    if (info.score > 0.85f) info.label = "Excellent";
    else if (info.score > 0.65f) info.label = "Strong";
    else if (info.score > 0.4f) info.label = "Fair";
    else info.label = "Weak";

    return info;
}
