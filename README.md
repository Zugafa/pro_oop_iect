# S.C.R.I.P.T. Password Manager

![C++](https://img.shields.io/badge/C++-20-blue.svg) ![SFML](https://img.shields.io/badge/GUI-SFML%203.0-green.svg) ![License](https://img.shields.io/badge/License-Proprietary-red.svg) [![Changelog](https://img.shields.io/badge/Changelog-History-blue)](CHANGELOG.md)

> **S.C.R.I.P.T.** (Secure, Centralized, Reliable, Identity, Password, Tool) is an advanced C++ software tool designed as a secure, centralized solution for managing passwords and digital identities.
---

## 💡 Project Idea

In the digital era, data security isn’t optional. Users juggle numerous accounts, and memorizing or storing them insecurely is a major risk.

**S.C.R.I.P.T.** proposes a robust system, built on modern software engineering principles, to store and organize sensitive data. If earlier versions focused on OOP logic, **Milestone 3** turns the project into a **full Desktop application**, offering an intuitive visual experience and data persistence.

---

## 🚀 Features & What’s New (v3.0 - Milestone 3)

The current version is a big jump from console to a modern graphical interface.

### 🪟 Graphical Interface (GUI)
- **Modern Design:** Built with **SFML 3.0**, the app window offers a professional look.
- **Intuitive Navigation:** Lateral sidebar, interactive cards for items, and dynamic edit forms.
- **Visual Feedback:** Custom cursor, text selection, real-time validation.

### 💾 Data Persistence
- **Auto-Save:** Data is automatically saved in a binary file (`vault.dat`).
- **Security:** The file is encrypted using **XOR Encryption** at the bit level, making it unreadable in standard text editors.
- **Serialization:** Custom serialization system based on `std::map`.

### 🛡️ Live Monitoring
- **Security Monitor:** A dedicated module analyzes password strength and card validity in real time.
- **Alerts:** Displays WARN/CRITICAL alerts directly in the "Security Report" dashboard.

---

## 🏛️ Architecture & Design Patterns

The project showcases advanced C++ use and well-known design patterns.

### 1. Design Patterns
- **Singleton:** Used for global singletons (`GestionarParole`, `ResourceManager`, `Configuratie`, `SeifFactory`).
- **Factory Pattern:** `SeifFactory` centralizes creation of polymorphic objects, essential for loading data from file without knowing their type at compile time.
- **Observer Pattern:** Implemented via `SecurityMonitor` (Observer) listening to `Utilizator` (Subject) to update the security report.
- **Prototype Pattern:** `clone()` ensures deep copy for polymorphic objects.

### 2. Generic Programming (Templates)
- **`Manager<T>`:** Template class for managing generic collections (users, history).

### 3. OOP & Memory Management
- **Smart Pointers:** Replaced `std::unique_ptr` with `std::shared_ptr`.
- **Custom Exceptions:** Minor changes.

---

## 📜 Changelog & Version History
Please refer to **[CHANGELOG.md](CHANGELOG.md)** for a detailed list of all changes, new features, and bug fixes implemented in the latest build.

---

## 📜 License
This software is proprietary. Viewing and running are allowed, but **modifying or redistributing the code is strictly prohibited**. See the [LICENSE](LICENSE) file for details.

---

## 🏗️ Build & Run Instructions

The project requires a **C++20**-compatible compiler and the **SFML 3.0** library.

### Prerequisites
Ensure SFML is installed and correctly configured in `CMakeLists.txt`.

### Build
```bash
# 1. Create build directory
cmake -B cmake-build-debug

# 2. Compile the project 
# (This step will automatically copy the 'images' and 'fonts' folders next to the executable)
cmake --build cmake-build-debug