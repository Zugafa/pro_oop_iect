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

const std::string& Seif::getEticheta() const
{ return eticheta; }

void Seif::setEticheta(const std::string& eticheta)
{ this->eticheta = eticheta; }
