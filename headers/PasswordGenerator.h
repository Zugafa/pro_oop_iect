/**
 * @file PasswordGenerator.h
 * @brief Utility for generating passwords with configurable character sets.
 */
#ifndef OOP_PASSWORDGENERATOR_H
#define OOP_PASSWORDGENERATOR_H

#include <string>

/**
 * @class PasswordGenerator
 * @brief Builds random passwords using customizable options.
 */
class PasswordGenerator
{
public:
    /**
     * @brief Generation options mirroring the UI toggles.
     */
    struct Options
    {
        int length = 16;
        bool useLower = true;
        bool useUpper = true;
        bool useDigits = true;
        bool useSymbols = true;
        bool easyToType = false; ///< If true, uses a reduced, less ambiguous alphabet.
    };

    /**
     * @brief Strength descriptor for a generated or existing password.
     */
    struct StrengthInfo
    {
        std::string label; ///< Human-readable label (Weak/Fair/Strong/Excellent).
        float score = 0.0f; ///< Numeric score (0-1) for UI color coding.
    };

    /**
     * @brief Generates a password using the provided options.
     * @param opt Options toggles (length, character sets).
     * @return The generated password string.
     */
    static std::string generate(const Options& opt);

    /**
     * @brief Evaluates strength heuristically (length + diversity).
     * @param pwd Password to evaluate.
     * @return StrengthInfo descriptor.
     */
    static StrengthInfo evaluateStrength(const std::string& pwd);
};

#endif //OOP_PASSWORDGENERATOR_H
