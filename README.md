# S.C.R.I.P.T. Password Manager

![C++](https://img.shields.io/badge/C++-20-blue.svg) ![SFML](https://img.shields.io/badge/GUI-SFML%203.0-green.svg) ![License](https://img.shields.io/badge/License-Proprietary-red.svg)

**S.C.R.I.P.T.** (Secure, Centralized, Reliable, Identity, Password, Tool) este un instrument software avansat dezvoltat în C++, conceput ca o soluție securizată și centralizată pentru gestionarea parolelor și identităților digitale.

---

## 💡 Ideea Proiectului

În era digitală, securitatea datelor nu este opțională. Utilizatorii jonglează cu numeroase conturi, iar memorarea sau stocarea nesigură a acestora reprezintă un risc major.

**S.C.R.I.P.T.** propune un sistem robust, fundamentat pe principii moderne de inginerie software, pentru a stoca și organiza date sensibile. Dacă în versiunile anterioare accentul a fost pe logica OOP, **Milestone 3** transformă proiectul într-o **aplicație Desktop completă**, oferind o experiență vizuală intuitivă și persistența datelor.

---

## 🚀 Funcționalități și Noutăți (v1.0 - Milestone 3)

Versiunea curentă reprezintă un salt major al distribuției, migrând de la consolă la o interfață grafică modernă.

### 🖥️ Interfață Grafică (GUI)
* **Design Modern:** Construită cu **SFML 3.0**, fereastra aplicației oferă un aspect profesional.
* **Navigare Intuitivă:** Sidebar lateral, carduri interactive pentru obiecte și formulare dinamice de editare.
* **Feedback Vizual:** Cursor custom, selecție text, validare în timp real.

### 💾 Persistența Datelor
* **Salvare Automată:** Datele sunt salvate automat într-un fișier binar (`vault.dat`).
* **Securitate:** Fișierul este criptat folosind **XOR Encryption** la nivel de bit, făcându-l ilizibil în editoare text standard.
* **Serializare:** Sistem propriu de serializare bazat pe `std::map`.

### 🛡️ Monitorizare Live
* **Security Monitor:** Un modul dedicat analizează în timp real tăria parolelor și validitatea cardurilor.
* **Alerte:** Afișează avertismente (WARN/CRITICAL) direct în dashboard-ul "Security Report".

---

## 🏛️ Arhitectură și Design Patterns

Proiectul demonstrează utilizarea avansată a C++ și a șabloanelor de proiectare consacrate.

### 1. Design Patterns
* **Singleton:** Utilizat pentru clasele unice globale (`GestionarParole`, `ResourceManager`, `Configuratie`, `SeifFactory`).
* **Factory Pattern:** `SeifFactory` centralizează crearea obiectelor polimorfice, esențial pentru încărcarea datelor din fișier fără a cunoaște tipul lor la compilare.
* **Observer Pattern:** Implementat prin `SecurityMonitor` (Observer) care ascultă modificările din `Utilizator` (Subject) pentru a actualiza raportul de securitate.
* **Prototype Pattern:** Metoda `clone()` asigură deep copy pentru obiectele polimorfice.

### 2. Programare Generic (Templates)
* **`Manager<T>`:** Clasă șablon pentru gestionarea colecțiilor generice (utilizatori, istoric).
* **`gasesteDupaNume<T>`:** Funcție șablon pentru căutarea elementelor în diverse containere, gestionând atât obiecte cât și smart pointers.

### 3. OOP & Memory Management
* **Smart Pointers:** Înlocuirea `std::unique_ptr` cu `std::shared_ptr`.
* **Excepții Custom:** Mici modificări.

---
## 📝 Licență
Acest software este proprietar. Vizualizarea și rularea sunt permise, dar **modificarea sau redistribuirea codului sunt strict interzise**.
Consultați fișierul [LICENSE](LICENSE) pentru detalii.
---

## 🛠️ Instrucțiuni de Compilare și Rulare

Proiectul necesită un compilator compatibil **C++20** și biblioteca **SFML 3.0**.

### Pre-rechizite
Asigurați-vă că SFML este instalat și configurat corect în `CMakeLists.txt`.

### Build
```bash
# 1. Crearea directorului de build
cmake -B cmake-build-debug

# 2. Compilarea proiectului 
# (Acest pas va copia automat folderele 'images' și 'fonts' lângă executabil)
cmake --build cmake-build-debug