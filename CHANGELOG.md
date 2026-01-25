# Changelog

Toate modificările notabile aduse acestui proiect vor fi documentate în acest fișier.

## [Milestone 3] - 2026-01-25

### 🚀 Adăugat (New Features)
- **UI/UX:**
    - Sistem de **Scroll Vertical** în Vault cu limitare automată (clamping) pentru navigarea listelor lungi.
    - Uniformizarea butoanelor: Toate acțiunile principale (Save, Add) sunt acum grupate în colțul dreapta-jos.
    - Suport pentru tasta **ENTER** pentru salvare rapidă și **TAB** pentru navigare între câmpuri.
    - Buton de "Cancel/Back" cu stil Outline pentru o ierarhie vizuală mai clară.
- **Backend:**
    - Implementare completă a câmpurilor `URL` și `Notes` în clasa `DateAutentificare`.
    - Mapare extinsă în `SeifFactory` pentru a suporta cele 5 câmpuri ale conturilor.

### 🐛 Fixes
- **Persistență:** Rezolvat bug-ul unde câmpurile opționale (URL, Note) nu erau salvate/încărcate corect din `vault.dat`.
- **Compilare:** Rezolvat erorile de `undefined reference` pentru setterii `setter_url` și `setter_note`.
- **Navigare:** Resetarea automată a scroll-ului la 0 la schimbarea categoriei pentru a preveni erorile de afișare.
- **Logică:** Corectarea constructorilor pentru a elimina avertismentele de "shadowing".

### 🔧 Tehnic
- Refactorizare `main.cpp` pentru a separa logica de desenare a butoanelor.
- Actualizare format binar `vault.dat`.