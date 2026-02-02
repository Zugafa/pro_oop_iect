#ifndef OOP_SECURITYMONITOR_H
#define OOP_SECURITYMONITOR_H
#include "Observer.h"
#include <vector>
#include <string>
#include <unordered_map>

class Utilizator;

/**
 * @class SecurityMonitor
 * @brief Concrete observer that monitors a user's vault for security events.
 */
class SecurityMonitor : public Observer {
public:
    /**
     * @brief Alert severity levels rendered in the Security report.
     */
    enum class Severity { Info, Warning, Critical };

    /**
     * @brief Structured security alert with severity and message.
     */
    struct SecurityAlert
    {
        Severity severity;
        std::string mesaj;
    };

    /**
     * @brief Breach lookup result for a password (HIBP range API).
     */
    struct BreachInfo
    {
        bool checked = false;
        bool onlineAvailable = false;
        bool pwned = false;
        int count = 0;
        std::string statusMsg;
    };

private:
    Utilizator& subiect;             ///< Reference to the observed user
    std::vector<std::string> alerte; ///< Legacy list of alert strings
    std::vector<SecurityAlert> alertDetails; ///< Structured alerts with severity
    std::unordered_map<std::string, BreachInfo> breachCache; ///< Eticheta -> breach result
    int duplicateCount;

    void addAlert(Severity sev, const std::string& msg);
    /**
     * @brief Online breach check for an arbitrary password (no caching).
     */
    [[nodiscard]] BreachInfo checkPasswordOnline(const std::string& pwd) const;

public:
    /**
     * @brief Constructor for SecurityMonitor.
     * @param u The user to monitor.
     */
    explicit SecurityMonitor(Utilizator& u);

    /**
     * @brief Called when the user's vault changes.
     */
    void update() override;

    /**
     * @brief Returns the list of security alerts.
     * @return Reference to the vector of alert strings.
     */
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::vector<std::string>& getAlerte() const { return alerte; }

    /**
     * @brief Returns structured alerts with severity metadata.
     */
    [[nodiscard]] const std::vector<SecurityAlert>& getAlertDetails() const { return alertDetails; }

    /**
     * @brief Returns cached breach info for a vault item label.
     */
    [[nodiscard]] BreachInfo getBreachInfo(const std::string& eticheta) const;

    /**
     * @brief Online breach check for a given password (no caching).
     */
    [[nodiscard]] BreachInfo runLiveBreachCheck(const std::string& pwd) const { return checkPasswordOnline(pwd); }

    /**
     * @brief Returns the number of items sharing duplicate passwords.
     */
    // cppcheck-suppress unusedFunction
    [[nodiscard]] int getDuplicateCount() const { return duplicateCount; }
};


#endif //OOP_SECURITYMONITOR_H
