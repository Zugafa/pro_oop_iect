#include "Seif.h"

inline std::ostream& operator<<(std::ostream& os, const Seif& s)
{
    s.afiseaza(os);
    return os;
}