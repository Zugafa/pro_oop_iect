#ifndef OOP_SECURITYMONITOR_H
#define OOP_SECURITYMONITOR_H
#include "Observer.h"
#include <vector>
#include <string>

class Utilizator;

class SecurityMonitor : public Observer {
    Utilizator& subiect;
    std::vector<std::string> alerte;

public:
    explicit SecurityMonitor(Utilizator& u);
    void update() override;
    [[nodiscard]] const std::vector<std::string>& getAlerte() const { return alerte; }
};


#endif //OOP_SECURITYMONITOR_H