# S.C.R.I.P.T.

> **S.C.R.I.P.T.** (**S**ecure, **C**entralized, **R**eliable, **I**dentity, **P**assword, **T**ool) este un instrument software dezvoltat în C++, conceput ca o soluție securizată și centralizată pentru gestionarea parolelor și identităților digitale.

---

## 💡 Ideea Proiectului

În era digitală, fiecare utilizator jonglează cu numeroase conturi online, fiecare necesitând date de autentificare unice și sigure. Memorarea sau gestionarea nesigură a acestora (de ex., în fișiere text simple) reprezintă un risc major de securitate.

**S.C.R.I.P.T.** propune un sistem robust, fundamentat pe principii de Programare Orientată pe Obiect (OOP), pentru a stoca, organiza și proteja datele sensibile. Aplicația permite utilizatorilor să își salveze toate datele de autentificare într-un singur loc, protejat și structurat logic.

---

## 🚀 Funcționalități și Noutăți (M2)

Versiunea curentă (v0.2) extinde funcționalitatea aplicației prin introducerea unui sistem polimorfic de stocare și a unui management robust al erorilor:

* **Seif Polimorfic:** Aplicația nu mai stochează doar parole. Utilizatorul poate adăuga acum diverse tipuri de obiecte securizate în același seif:
    * **Conturi** (`DateAutentificare`)
    * **Identități** (`Identitate` - ex. date personale, adrese)
    * **Carduri Bancare** (`CardBancar`)
    * **Notițe Securizate** (`NotitaSecurizata`)
* **Audit de Securitate:** Fiecare tip de obiect are propria logică de verificare a securității (ex: verificarea tăriei parolei pentru conturi, verificarea formatului pentru carduri).
* **Management Automat al Memoriei:** Utilizarea **Smart Pointers** (`std::unique_ptr`) și a containerelor **STL** (`std::vector`) elimină riscul de memory leaks și erori de gestionare a resurselor.
* **Sistem Avansat de Erori:** În loc de mesaje simple în consolă, aplicația folosește o ierarhie de **Excepții Custom** care oferă nu doar diagnosticul erorii, ci și sugestii inteligente pentru rezolvarea ei (ex: "Parola e prea scurtă, mai adaugă X caractere").
* **Criptare și Decriptare:** Funcționalități specifice disponibile pentru conturile de autentificare, accesate dinamic prin `dynamic_cast`.

---

## 🏛️ Arhitectura Curentă (Partea II)

Milestone 2 a transformat arhitectura într-una bazată pe **Moștenire și Polimorfism**:

### 1. Ierarhia de Clase (Moștenire)
La baza sistemului stă clasa abstractă **`Seif`**, care definește contractul pentru orice obiect stocabil (interfață pur virtuală).
* **`Seif` (Bază Abstractă):** Definește metodele virtuale pure (`clone`, `getTip`, `afiseaza`, `verificaSecuritate`) și gestionează atribute statice (contor obiecte).
* **Clase Derivate:** Implementează comportamente specifice:
    * `DateAutentificare`: Include logica de criptare Vigenère și validare parole.
    * `Identitate`, `CardBancar`, `NotitaSecurizata`: Stochează date specifice și implementează afișarea și auditul propriu.

### 2. Managementul Resurselor (Utilizator)
Clasa `Utilizator` a fost refactorizată complet pentru a respecta principiile C++ Modern:
* **STL & Smart Pointers:** Stochează obiectele într-un `std::vector<std::unique_ptr<Seif>>`.
* **Polimorfism:** Permite adăugarea, ștergerea și afișarea eterogenă a obiectelor.
* **Copy-and-Swap:** Implementează corect copierea profundă (Deep Copy) folosind idiomul "Virtual Constructor" (`clone()`).

### 3. Ierarhia de Excepții
Gestionarea erorilor se face printr-o ierarhie dedicată derivată din `std::exception`:
* **`ScriptException` (Bază):** Definește interfața pentru excepții cu sugestii (`getSugestie()`).
* **Derivate:** `EroareValidare` (calcul matematic pentru validare), `ObiectNegasit` (căutare eșuată), `EroareAcces` (index out of bounds).

---

## 🛠️ Instrucțiuni de Compilare și Rulare

Proiectul folosește CMake pentru build.

```bash
# 1. Crearea directorului de build (dacă nu există)
cmake -B build

# 2. Compilarea proiectului
cmake --build build

# 3. Rularea scenariului de testare (din Partea II)
./build/oop
