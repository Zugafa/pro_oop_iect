# Changelog

All notable changes to the **S.C.R.I.P.T Password Manager** project since Milestone 3 (M3).

## [Unreleased] - Current Build

### 🚀 Added (New Features)
* **Advanced Input System:**
  * Implemented a robust event processing engine supporting text navigation and editing.
  * **Shortcuts:** Added support for `Ctrl+C`, `Ctrl+V`, `Ctrl+X`, `Ctrl+A`.
  * **Undo/Redo:** Implemented `Ctrl+Z` / `Ctrl+Y` stack for both Edit Forms and Search Bar.
  * **Navigation:** Added word-jumping (`Ctrl + Left/Right`) and selection (`Shift + Arrows`).
* **Search System Upgrade:**
  * Search bar now supports full cursor positioning, selection, and editing.
  * Search query automatically resets when switching sidebar categories.
* **Sorting System:**
  * Added a **Sort Modal** window with dynamic dimension calculation.
  * Implemented sorting by **Title**, **Cardholder Name**, and **Expiry Date**.
  * Added Ascending/Descending toggle.
* **UI Interactivity:**
  * **Scroll Support:** Added vertical scrolling for the Vault list using the mouse wheel.
  * **Context Menus:** Added "..." buttons on cards with actions: *Copy (User/Pass/CVV)*, *Go to URL*, *Edit*, *Delete*.
  * **Toast Notifications:** Added non-intrusive popup messages (e.g., "Copied to clipboard") that fade out automatically.
  * **Confirmation Modals:** Added "Are you sure?" overlay when deleting items to prevent accidental data loss.
* **Cardholder Field:** Updated `CardBancar` data model to explicitly store and display the **Cardholder Name**.

### 🛠 Changed (Improvements)
* **UI/UX Redesign:**
  * Refined the Sidebar visual style (Color: `CLR_NAVY`).
  * Increased main window resolution to **1200x900**.
  * Standardized button sizes and positions (Action buttons aligned to bottom-right).
* **Visual Tweaks:**
  * **Secure Notes:** Modified the list view for Notes to display *only* the Title (centered), removing redundant secondary text.
  * **Z-Order Rendering:** Fixed rendering layers so the Header acts as a mask over the scrolling content (preventing items from floating above the search bar).
* **Search Logic:** Search now filters by Title, Username, URL, and Cardholder Name.

### 🐛 Fixed (Bug Fixes)
* **Click-Through Logic:** Fixed a critical issue where clicks would pass through Modals (Sort/Delete) and trigger buttons in the background.
* **Data Model Constructor:** Fixed parameter order in `CardBancar` constructor which caused data misalignment (Title swapping with Card Number).
* **Input Focus:** Fixed issues where keyboard shortcuts were not triggering correctly inside the Search Bar.
* **Persistence:** Resolved issues with saving/loading the new `numeDetinator` field in `vault.dat`.
* **Error Handling:** Added dedicated catch block for `EroareFisier` in `main.cpp` with Windows MessageBox support for missing resource files.

### 🧩 Technical Details
* Refactored `UserInterface::processEvents` to handle complex keyboard states (Ctrl/Shift modifiers).
* Refactored `UserInterface::drawSortModal` to auto-calculate height and center itself on screen based on the number of active options.
* Updated `SeifFactory` to handle backward compatibility when loading older data files without the `holder` field.