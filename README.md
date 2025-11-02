# S.C.R.I.P.T.

> **S.C.R.I.P.T.** (**S**ecure, **C**entralized, **R**eliable, **I**dentity, **P**assword, **T**ool) este un instrument software dezvoltat în C++, conceput ca o soluție securizată și centralizată pentru gestionarea parolelor și identităților digitale.

---

## 💡 Ideea Proiectului

În era digitală, fiecare utilizator jonglează cu numeroase conturi online, fiecare necesitând date de autentificare unice și sigure. Memorarea sau gestionarea nesigură a acestora (de ex., în fișiere text simple) reprezintă un risc major de securitate.

**S.C.R.I.P.T.** propune un sistem robust, fundamentat pe principii de Programare Orientată pe Obiect (OOP), pentru a stoca, organiza și proteja datele sensibile. Aplicația permite utilizatorilor să își salveze toate datele de autentificare într-un singur loc, protejat și structurat logic.

---

## 🚀 Funcționalități (MI)

Versiunea curentă (v0.1) stabilește fundația aplicației și include următoarele funcționalități de bază:

* **Managementul Utilizatorilor:** Permite crearea unui profil de utilizator (`Utilizator`) care poate deține o colecție de date de autentificare.
* **Managementul Conturilor:** Implementează logica de bază pentru adăugarea și ștergerea datelor de autentificare (`DateAutentificare`) din profilul unui utilizator.
* **Inserare Sortată:** Toate conturile adăugate în profilul unui utilizator sunt **sortate automat alfabetic** (după numele platformei) pentru o organizare logică.
* **Validarea Parolelor:** La adăugarea unui cont nou, sistemul validează parola conform unor reguli de securitate (stabilite în `Configuratie`) și **respinge** conturile care nu îndeplinesc cerințele minime (ex. lungime).
* **Criptare (Proof-of-Concept):** Include un mecanism de criptare (Cifrul Vigenère) pentru a demonstra securizarea parolelor stocate.
* **Configurare Centralizată:** Regulile de securitate (cheia de criptare, lungimea minimă a parolei, caracterele speciale permise) sunt stocate centralizat într-o clasă de configurare, permițând modificarea facilă a politicilor de securitate.
* **Management Robust al Memoriei:** Asigură o gestionare corectă a memoriei pentru colecțiile dinamice de conturi prin implementarea corectă a **Regulii celor Trei** (Constructor de Copiere, `operator=`, Destructor), garantând o "Deep Copy" și prevenind memory leaks.

---

## 🏛️ Arhitectura Curentă (Partea I)

Fundația proiectului (Partea I) este construită pe **principiul compunerii** și este formată din 3 clase principale:

### 1. Clasa `Configuratie`
Stochează și gestionează centralizat toate regulile de securitate și setările aplicației (ex. cheia Vigenère, lungimea minimă a parolei).

### 2. Clasa `DateAutentificare`
Reprezintă un singur set de date de autentificare (platformă, username, parolă).
* **Compunere:** *Are o* `Configuratie` pentru a ști cum să valideze sau să cripteze datele.
* Implementează funcții netriviale precum `CriptareVigenere()` și `setter_parola()`.

### 3. Clasa `Utilizator`
Reprezintă profilul utilizatorului care deține conturile.
* **Compunere:** *Are o* `Configuratie` (pentru a o pasa conturilor noi) și *are* un array alocat dinamic de `DateAutentificare*`.
* Implementează **Regula celor Trei** pentru managementul corect al memoriei (Deep Copy).
* Implementează funcții netriviale complexe, precum `adaugaCont()` (cu realocare de memorie și inserare sortată) și `stergeCont()` (cu căutare și `shift-left`).

---

## 🛠️ Instrucțiuni de Compilare și Rulare

Proiectul folosește CMake pentru build.

```bash
# 1. Crearea directorului de build (dacă nu există)
cmake -B build

# 2. Compilarea proiectului
cmake --build build

# 3. Rularea scenariului de testare (din Partea I)
./build/S.C.R.I.P.T
