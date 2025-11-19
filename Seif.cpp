#include "Seif.h"

int Seif::nrObiecteTotale = 0;

Seif::Seif(const std::string& eticheta) : eticheta(eticheta)
{
    nrObiecteTotale++;
}

Seif::~Seif()
{
    nrObiecteTotale--;
}

int Seif::getNumarObiecteTotale()
{
    return nrObiecteTotale;
}
