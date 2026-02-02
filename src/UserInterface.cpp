/**
 * @file UserInterface.cpp
 * @brief Implementation of the main graphical user interface wrapper.
 *
 * Handles the application loop, event processing, rendering, and interaction
 * between the user and the backend systems (GestionarParole, ResourceManager).
 *
 * @author Andrei Nituica
 * @date 2026
 */

#include "UserInterface.h"
#include "ResourceManager.hpp"
#include "Configuratie.h"
#include "SeifFactory.h"
#include "DateAutentificare.h"
#include "CardBancar.h"
#include "NotitaSecurizata.h"
#include "Identitate.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <SFML/Window/Clipboard.hpp>
#include <array>
#include <sstream>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

static constexpr int DEFAULT_GEN_LEN = 16;

/**
 * @brief Destructor.
 *
 * Automatically saves the current vault state to disk upon application exit
 * to prevent data loss. Catches validation or IO errors to ensure a clean exit.
 */
UserInterface::~UserInterface()
{
    try
    {
        std::cout << "[INFO] Shutting down... performing autosave.\n";
        GestionarParole::getInstance().salveazaDatelePeDisc();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ERROR] Failed to save data during shutdown: " << e.what() << std::endl;
    }
}

/**
 * @brief Constructor.
 *
 * Initializes the window, links resources from the ResourceManager Singleton,
 * and sets up the initial application state.
 */
UserInterface::UserInterface()
    : window(sf::VideoMode({1200, 900}), "S.C.R.I.P.T. Password Manager"),
      font(ResourceManager::Instance().getFont("FiraSans-Regular.ttf")),
      logoSprite(ResourceManager::Instance().getTexture("logo.png")),
      logoText(font, "S.C.R.I.P.T", 20),

      // Iconite (respect order of declaration)
      iconMenu(ResourceManager::Instance().getTexture("icon_menu.png")),
      iconCopy(ResourceManager::Instance().getTexture("icon_copy.png")),
      iconGlobe(ResourceManager::Instance().getTexture("icon_globe.png")),
      iconEdit(ResourceManager::Instance().getTexture("icon_edit.png")),
      iconTrash(ResourceManager::Instance().getTexture("icon_trash.png")),
      iconEyeOpen(ResourceManager::Instance().getTexture("icon_ochiD.png")),
      iconEyeClosed(ResourceManager::Instance().getTexture("icon_ochiI.png")),
      iconCheck(ResourceManager::Instance().getTexture("icon_check.png")),
      iconRenew(ResourceManager::Instance().getTexture("icon_renew.png")),
      iconBeforeCheck(ResourceManager::Instance().getTexture("icon_beforeCheck.png")),
      icon_search(ResourceManager::Instance().getTexture("icon_search.png")),
      icon_sort(ResourceManager::Instance().getTexture("icon_sort.png")),

      // Stari aplicatie
      appState(AppState::Vault),
      currentCat(Category::Accounts),
      activeItem(nullptr),
      menuOpenItem(nullptr),
      menuPos({0.f, 0.f}),

      // Input
      focusKey(""),
      guiError(""),

      // Flags
      scrollOffset(0.f),
      showPass(false), showCVV(false), showCursor(true), triggerSave(false),

      // Clocks & Mouse
      click(false), dblClick(false), mPos({0.f, 0.f}),
      genSliderDirty(false), pendingGenLength(16),

      // Toast & Delete
      showDeleteConfirm(false),
      itemPendingDelete(nullptr),

      // Search (DECLARAT INAINTE DE TEXTE IN .H, DECI INITIALIZAT INAINTE)
      searchQuery(""),
      isSearchFocused(false),
      searchBg(),
    searchText(font, "", 17),
    searchPlaceholder(font, "Search...", 17),
      searchCursor(0),
      searchAnchor(0),

      // Sort State
      showSortModal(false),
      sortModalArmed(false),
      activeSortOrder(SortOrder::Ascending),
      activeCriteria(SortCriteria::Label),
      tempSortOrder(SortOrder::Ascending),
      tempCriteria(SortCriteria::Label),

      // Monitor
      monitor(nullptr),
      securityNeedsRefresh(true),

      // Password Generator
      showPassGenModal(false),
      showInlinePassGen(false),
      passGenModalArmed(false),
      inlineDraggingLen(false),
      inlineWasDraggingLen(false),
      modalDraggingLen(false),
      modalWasDraggingLen(false),
      backToSecurity(false),
      showBreachPanel(false),
      showSecurityGenPanel(false),
      showDupPanel(false),
      showWeakPanel(false),
      genOptions(),
      genStrength(),
      generatedPass(""),
      generatorTargetKey(""),
      hasLiveBreachInfo(false),
      liveBreachInfo(),
      generatedColors()
{
    window.setFramerateLimit(60);
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    genOptions.length = DEFAULT_GEN_LEN;
    pendingGenLength = DEFAULT_GEN_LEN;
    initVisuals();
    initMockData();

    regeneratePassword();
}

/**
 * @brief Initializes the backend and loads data.
 *
 * Creates a default user and loads data from 'vault.dat'.
 * If the vault is empty, it populates it with example data.
 */
void UserInterface::initMockData()
{
    auto& app = GestionarParole::getInstance();
    try
    {
        // Create default user and login
        app.getManager().adauga(Utilizator("Andrei", "andrei@proiect.ro", "secret123"));
        app.login("Andrei", "secret123");

        if (Utilizator* u = app.getUserCrt())
        {
            // Attach the security monitor observer
            static SecurityMonitor m(*u);
            monitor = &m;
            u->attach(monitor);

            // Load persistence file
            app.incarcaDateleDePeDisc();

            // If empty, add mock data for demonstration
            if (u->getNrObiecte() == 0)
            {
                u->adaugaObiect(std::make_shared<DateAutentificare>("Facebook", "andrei.nituica", "ParolaValida123!",
                                                                    "https://www.facebook.com/", "Main Account"));
                u->adaugaObiect(std::make_shared<CardBancar>(
                    "Revolut",
                    "Nituica Andrei-Sebastian", "4556 1234 5678 0000",
                    "12/28",
                    "123"
                ));
                u->adaugaObiect(std::make_shared<NotitaSecurizata>("Gate Code", "Access: 9988#"));
                u->adaugaObiect(std::make_shared<Identitate>("Home", "Nituica", "Andrei", "0720xxx", "a@m.ro", "Str. X",
                                                             "Otopeni", "IF", "RO", "0771"));
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ERROR] Init failed: " << e.what() << std::endl;
    }
}

/**
 * @brief The main application loop.
 */
void UserInterface::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

/**
 * @brief Configures the visual properties of a UI icon.
 *
 * Sets the smoothing, scale, origin (centering), and color of the sprite.
 * Adapted for SFML 3.0.0 (Reference handling and Vector2f parameters).
 *
 * @param sprite Reference to the sf::Sprite to configure.
 * @param texName The name of the texture file (used for conditional sizing).
 */
void UserInterface::setupIcon(sf::Sprite& sprite, const std::string& texName)
{
    const auto& tex = sprite.getTexture();

    // We cast away const from the reference to enable smoothing
    const_cast<sf::Texture&>(tex).setSmooth(true);

    // Standard size for action icons (24px), slightly smaller for the menu dots (20px)
    float targetSize = (texName == "icon_menu.png") ? 20.f : 24.f;

    float texW = static_cast<float>(tex.getSize().x);
    float scale = targetSize / (texW > 0 ? texW : 1.f);

    sprite.setScale({scale, scale});

    sprite.setOrigin({tex.getSize().x / 2.f, tex.getSize().y / 2.f});

    // Apply a dark gray tint for a professional look (RGB: 80, 80, 90)
    sprite.setColor(sf::Color(80, 80, 90));
}

/**
 * @brief Configures visual properties of UI elements.
 *
 * Sets the scale, position, and color of the logo and header text.
 * Does NOT load files (that is handled by ResourceManager in the constructor).
 */
void UserInterface::initVisuals()
{
    // ... (Codul pentru logo si sidebar ramane la fel) ...
    const auto& tex = logoSprite.getTexture();
    float targetHeight = 120.f;
    float texH = static_cast<float>(tex.getSize().y);
    float scaleFactor = targetHeight / (texH > 0 ? texH : 1.f);
    logoSprite.setScale({scaleFactor, scaleFactor});
    logoSprite.setPosition({-5.f, 25.f});

    logoText.setFillColor(sf::Color::White);
    auto logoBounds = logoSprite.getGlobalBounds();
    logoText.setPosition({
        logoBounds.position.x + logoBounds.size.x, logoBounds.position.y + (logoBounds.size.y / 2.f) - 15.f
    });

    // Setup Helpers
    setupIcon(iconMenu, "icon_menu.png");
    setupIcon(iconCopy, "icon_copy.png");
    setupIcon(iconGlobe, "icon_globe.png");
    setupIcon(iconEdit, "icon_edit.png");
    setupIcon(iconTrash, "icon_trash.png");
    setupIcon(iconEyeOpen, "icon_ochiD.png");
    setupIcon(iconEyeClosed, "icon_ochiI.png");
    setupIcon(iconCheck, "icon_check.png");
    setupIcon(iconBeforeCheck, "icon_beforeCheck.png");
    setupIcon(iconRenew, "icon_renew.png");

    // --- [NOU] SEARCH BAR CONFIG (LARGIT) ---
    float startX = SIDE_W + 30.f;
    float startY = 80.f;

    // 1. Bara de Search (600px)
    searchBg.setSize({600.f, 40.f});
    searchBg.setFillColor(sf::Color::White);
    searchBg.setOutlineThickness(1.5f);
    searchBg.setOutlineColor(sf::Color(200, 200, 200));
    searchBg.setPosition({startX, startY});

    // 2. Iconita lupa (resetam origin/scale manual sau prin setupIcon daca e adecvat)
    // Aici setam manual pentru control fin in bara
    icon_search.setTexture(ResourceManager::Instance().getTexture("icon_search.png"));
    icon_search.setScale({0.8f, 0.8f}); // Putin mai mica
    icon_search.setColor(sf::Color(100, 100, 100)); // Gri
    // Pozitionare in stanga barei
    icon_search.setPosition({startX + 10.f, startY + 10.f});

    // 3. Textul cautat si Placeholder
    searchText.setFillColor(sf::Color::Black);
    searchText.setPosition({startX + 45.f, startY + 9.f});

    searchPlaceholder.setFillColor(sf::Color(180, 180, 180));
    searchPlaceholder.setPosition({startX + 45.f, startY + 9.f});

    // --- [NOU] SORT BUTTON CONFIG ---
    // Il punem imediat dupa bara de search (600px + 10px spatiu)
    sortBtn.setSize({40.f, 40.f});
    sortBtn.setFillColor(sf::Color::White);
    sortBtn.setOutlineThickness(1.5f);
    sortBtn.setOutlineColor(sf::Color(200, 200, 200));
    sortBtn.setPosition({startX + 610.f, startY});

    // Iconita sortare centrata in buton
    setupIcon(icon_sort, "icon_sort.png"); // Folosim helper-ul pt centrare
    icon_sort.setPosition({sortBtn.getPosition().x + 20.f, sortBtn.getPosition().y + 20.f});
}

/**
 * @brief Handles all user input events (Keyboard, Mouse, Window).
 */
void UserInterface::processEvents()
{
    click = false;
    dblClick = false;
    mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (blinkClock.getElapsedTime().asSeconds() > 0.5f)
    {
        showCursor = !showCursor;
        blinkClock.restart();
    }

    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>()) window.close();

        // Scroll
        if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            if (showPassGenModal) continue;
            if (scroll->wheel == sf::Mouse::Wheel::Vertical && appState == AppState::Vault)
                scrollOffset -= scroll->delta * 25.f;
        }

        // --- MOUSE CLICK ---
        if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (m->button == sf::Mouse::Button::Left)
            {
                click = true;
                if (doubleClickClock.getElapsedTime().asSeconds() < 0.3f) dblClick = true;
                doubleClickClock.restart();

                if (showPassGenModal) continue;

                if (appState == AppState::Vault)
                {
                    // Click Search
                    if (currentCat != Category::Security && searchBg.getGlobalBounds().contains(mPos) && !showSortModal)
                    {
                        isSearchFocused = true;
                        searchBg.setOutlineColor(CLR_ACCENT);
                        searchBg.setOutlineThickness(2.f);
                        // Pozitionare cursor + Resetare selectie
                        searchCursor = getIndexAtMouse(searchText, mPos.x);
                        searchAnchor = searchCursor;
                    }
                    else if (currentCat != Category::Security)
                    {
                        isSearchFocused = false;
                        searchBg.setOutlineColor(sf::Color(200, 200, 200));
                        searchBg.setOutlineThickness(1.f);
                    }

                    // Click Sort
                    if (currentCat != Category::Security && sortBtn.getGlobalBounds().contains(mPos) && !showSortModal)
                    {
                        showSortModal = true;
                        sortModalArmed = false;
                        tempCriteria = activeCriteria;
                        tempSortOrder = activeSortOrder;
                    }
                }
            }
        }

        bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::RControl);
        bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);

        // --- 1. TEXT INPUT ---
        if (const auto* text = event->getIf<sf::Event::TextEntered>())
        {
            bool isBackspace = (text->unicode == 8);
            bool isPrintable = (text->unicode >= 32 && text->unicode < 128);
            if (showPassGenModal) continue;

            if (!ctrl || isBackspace)
            {
                // A. SEARCH BAR
                if (appState == AppState::Vault && isSearchFocused && !showSortModal)
                {
                    // Helper Undo Search
                    auto saveSearchUndo = [&]()
                    {
                        undoStack["$SEARCH$"].push_back(toSfStr(searchQuery));
                        redoStack["$SEARCH$"].clear();
                        if (undoStack["$SEARCH$"].size() > 50)
                            undoStack["$SEARCH$"].erase(
                                undoStack["$SEARCH$"].begin());
                    };

                    if (isBackspace)
                    {
                        if (searchAnchor != searchCursor || searchCursor > 0)
                        {
                            saveSearchUndo();
                            if (searchAnchor != searchCursor)
                            {
                                // Sterge selectia
                                size_t s = std::min(searchCursor, searchAnchor);
                                size_t e = std::max(searchCursor, searchAnchor);
                                searchQuery.erase(s, e - s);
                                searchCursor = searchAnchor = s;
                            }
                            else
                            {
                                // Sterge 1 char
                                searchQuery.erase(searchCursor - 1, 1);
                                searchCursor--;
                                searchAnchor = searchCursor;
                            }
                        }
                    }
                    else if (isPrintable)
                    {
                        if (searchQuery.size() < 50)
                        {
                            saveSearchUndo();
                            if (searchAnchor != searchCursor)
                            {
                                // Suprascrie selectia
                                size_t s = std::min(searchCursor, searchAnchor);
                                size_t e = std::max(searchCursor, searchAnchor);
                                searchQuery.erase(s, e - s);
                                searchCursor = searchAnchor = s;
                            }
                            searchQuery.insert(searchCursor, 1, static_cast<char>(text->unicode));
                            searchCursor++;
                            searchAnchor = searchCursor;
                        }
                    }
                }
                // B. FORMULARE
                else if (appState != AppState::Vault && !focusKey.empty())
                {
                    auto& cur = cursors[focusKey];
                    auto& anc = anchors[focusKey];
                    auto& buf = buffers[focusKey];

                    if (isBackspace)
                    {
                        if (anc != cur || cur > 0)
                        {
                            undoStack[focusKey].push_back(buffers[focusKey]);
                            redoStack[focusKey].clear();

                            if (anc != cur)
                            {
                                size_t s = std::min(anc, cur), e = std::max(anc, cur);
                                buf.erase(s, e - s);
                                cur = anc = s;
                            }
                            else
                            {
                                buf.erase(cur - 1, 1);
                                cur--;
                                anc = cur;
                            }
                        }
                    }
                    else if (isPrintable)
                    {
                        undoStack[focusKey].push_back(buffers[focusKey]);
                        redoStack[focusKey].clear();

                        if (anc != cur)
                        {
                            size_t s = std::min(anc, cur), e = std::max(anc, cur);
                            buf.erase(s, e - s);
                            cur = anc = s;
                        }
                        buf.insert(cur, text->unicode);
                        cur++;
                        anc = cur;
                    }
                }
            }
        }

        // --- 2. KEY PRESSED ---
        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (showPassGenModal)
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    showPassGenModal = false;
                    passGenModalArmed = false;
                    modalDraggingLen = false;
                    modalWasDraggingLen = false;
                    genOptions.length = DEFAULT_GEN_LEN;
                    pendingGenLength = DEFAULT_GEN_LEN;
                    genSliderDirty = false;
                    regeneratePassword();
                }
                continue;
            }
            if (showSortModal && key->code == sf::Keyboard::Key::Escape)
            {
                showSortModal = false;
                sortModalArmed = false;
                continue;
            }
            if (key->code == sf::Keyboard::Key::Escape && appState != AppState::Vault)
            {
                appState = AppState::Vault;
                guiError = "";
                showPass = false;
                showCVV = false;
                showInlinePassGen = false;
                showBreachPanel = false;
                showPassGenModal = false;
                passGenModalArmed = false;
                modalDraggingLen = false;
                modalWasDraggingLen = false;
                hasLiveBreachInfo = false;
                if (backToSecurity)
                {
                    currentCat = Category::Security;
                    backToSecurity = false;
                }
                continue;
            }
            // === NAVIGARE (SAGEATA STANGA / DREAPTA) ===
            if (key->code == sf::Keyboard::Key::Left)
            {
                // Search
                if (appState == AppState::Vault && isSearchFocused)
                {
                    if (ctrl && searchCursor > 0)
                    {
                        // Jump Word
                        size_t i = searchCursor;
                        while (i > 0 && searchQuery[i - 1] == ' ') i--;
                        while (i > 0 && searchQuery[i - 1] != ' ') i--;
                        searchCursor = i;
                    }
                    else if (searchCursor > 0) searchCursor--;

                    if (!shift) searchAnchor = searchCursor; // Update selection
                }
                // Formular
                else if (appState != AppState::Vault && !focusKey.empty())
                {
                    auto& cur = cursors[focusKey];
                    if (ctrl && cur > 0)
                    {
                        size_t i = cur;
                        while (i > 0 && buffers[focusKey][i - 1] == ' ') i--;
                        while (i > 0 && buffers[focusKey][i - 1] != ' ') i--;
                        cur = i;
                    }
                    else if (cur > 0) cur--;
                    if (!shift) anchors[focusKey] = cur;
                }
            }
            else if (key->code == sf::Keyboard::Key::Right)
            {
                // Search
                if (appState == AppState::Vault && isSearchFocused)
                {
                    if (ctrl && searchCursor < searchQuery.size())
                    {
                        size_t i = searchCursor;
                        while (i < searchQuery.size() && searchQuery[i] != ' ') i++;
                        while (i < searchQuery.size() && searchQuery[i] == ' ') i++;
                        searchCursor = i;
                    }
                    else if (searchCursor < searchQuery.size()) searchCursor++;

                    if (!shift) searchAnchor = searchCursor;
                }
                // Formular
                else if (appState != AppState::Vault && !focusKey.empty())
                {
                    auto& cur = cursors[focusKey];
                    size_t len = buffers[focusKey].getSize();
                    if (ctrl && cur < len)
                    {
                        size_t i = cur;
                        while (i < len && buffers[focusKey][i] != ' ') i++;
                        while (i < len && buffers[focusKey][i] == ' ') i++;
                        cur = i;
                    }
                    else if (cur < len) cur++;
                    if (!shift) anchors[focusKey] = cur;
                }
            }

            // === DELETE KEY (Sterge la DREAPTA) ===
            if (key->code == sf::Keyboard::Key::Delete)
            {
                // Search
                if (appState == AppState::Vault && isSearchFocused)
                {
                    undoStack["$SEARCH$"].push_back(toSfStr(searchQuery));
                    if (searchAnchor != searchCursor)
                    {
                        // Delete selection
                        size_t s = std::min(searchCursor, searchAnchor);
                        size_t e = std::max(searchCursor, searchAnchor);
                        searchQuery.erase(s, e - s);
                        searchCursor = searchAnchor = s;
                    }
                    else if (searchCursor < searchQuery.size())
                    {
                        searchQuery.erase(searchCursor, 1);
                    }
                }
                // Formular
                else if (appState != AppState::Vault && !focusKey.empty())
                {
                    auto& cur = cursors[focusKey];
                    auto& anc = anchors[focusKey];
                    auto& buf = buffers[focusKey];
                    undoStack[focusKey].push_back(buffers[focusKey]);

                    if (anc != cur)
                    {
                        size_t s = std::min(anc, cur), e = std::max(anc, cur);
                        buf.erase(s, e - s);
                        cur = anc = s;
                    }
                    else if (cur < buf.getSize())
                    {
                        buf.erase(cur, 1);
                    }
                }
            }

            // === CLIPBOARD & UNDO/REDO (CTRL + ...) ===
            if (ctrl)
            {
                // CTRL+A (Select All)
                if (key->code == sf::Keyboard::Key::A)
                {
                    if (appState == AppState::Vault && isSearchFocused)
                    {
                        searchAnchor = 0;
                        searchCursor = searchQuery.size();
                    }
                    else if (appState != AppState::Vault && !focusKey.empty())
                    {
                        anchors[focusKey] = 0;
                        cursors[focusKey] = buffers[focusKey].getSize();
                    }
                }
                // CTRL+C (Copy)
                else if (key->code == sf::Keyboard::Key::C)
                {
                    if (appState == AppState::Vault && isSearchFocused)
                    {
                        sf::Clipboard::setString(searchQuery);
                        // Simplificat: copiaza tot (sau poti implementa doar selectia)
                    }
                    else if (appState != AppState::Vault && !focusKey.empty())
                    {
                        sf::Clipboard::setString(u8ToStr(buffers[focusKey].toUtf8()));
                    }
                }
                // CTRL+V (Paste)
                else if (key->code == sf::Keyboard::Key::V)
                {
                    sf::String clip = sf::Clipboard::getString();
                    if (clip.getSize() > 0)
                    {
                        if (appState == AppState::Vault && isSearchFocused)
                        {
                            undoStack["$SEARCH$"].push_back(toSfStr(searchQuery));
                            std::string s = u8ToStr(clip.toUtf8());
                            if (searchQuery.size() + s.size() < 50)
                            {
                                if (searchAnchor != searchCursor)
                                {
                                    // Overwrite
                                    size_t start = std::min(searchCursor, searchAnchor);
                                    searchQuery.erase(start, std::max(searchCursor, searchAnchor) - start);
                                    searchCursor = searchAnchor = start;
                                }
                                searchQuery.insert(searchCursor, s);
                                searchCursor += s.size();
                                searchAnchor = searchCursor;
                            }
                        }
                        else if (appState != AppState::Vault && !focusKey.empty())
                        {
                            undoStack[focusKey].push_back(buffers[focusKey]);
                            auto& buf = buffers[focusKey];
                            auto& cur = cursors[focusKey];
                            auto& anc = anchors[focusKey];
                            if (anc != cur)
                            {
                                size_t s = std::min(anc, cur), e = std::max(anc, cur);
                                buf.erase(s, e - s);
                                cur = anc = s;
                            }
                            buf.insert(cur, clip);
                            cur += clip.getSize();
                            anc = cur;
                        }
                    }
                }
                // CTRL+Z (Undo)
                else if (key->code == sf::Keyboard::Key::Z)
                {
                    if (appState == AppState::Vault && isSearchFocused)
                    {
                        if (!undoStack["$SEARCH$"].empty())
                        {
                            redoStack["$SEARCH$"].push_back(toSfStr(searchQuery));
                            searchQuery = u8ToStr(undoStack["$SEARCH$"].back().toUtf8());
                            undoStack["$SEARCH$"].pop_back();
                            searchCursor = searchAnchor = searchQuery.size();
                        }
                    }
                    else if (appState != AppState::Vault && !focusKey.empty())
                    {
                        if (!undoStack[focusKey].empty())
                        {
                            redoStack[focusKey].push_back(buffers[focusKey]);
                            buffers[focusKey] = undoStack[focusKey].back();
                            undoStack[focusKey].pop_back();
                            cursors[focusKey] = anchors[focusKey] = buffers[focusKey].getSize();
                        }
                    }
                }
                // CTRL+Y (Redo)
                else if (key->code == sf::Keyboard::Key::Y)
                {
                    if (appState == AppState::Vault && isSearchFocused)
                    {
                        if (!redoStack["$SEARCH$"].empty())
                        {
                            undoStack["$SEARCH$"].push_back(toSfStr(searchQuery));
                            searchQuery = u8ToStr(redoStack["$SEARCH$"].back().toUtf8());
                            redoStack["$SEARCH$"].pop_back();
                            searchCursor = searchAnchor = searchQuery.size();
                        }
                    }
                    else if (appState != AppState::Vault && !focusKey.empty())
                    {
                        if (!redoStack[focusKey].empty())
                        {
                            undoStack[focusKey].push_back(buffers[focusKey]);
                            buffers[focusKey] = redoStack[focusKey].back();
                            redoStack[focusKey].pop_back();
                            cursors[focusKey] = anchors[focusKey] = buffers[focusKey].getSize();
                        }
                    }
                }
            }

            // === TAB (Navigare) ===
            if (key->code == sf::Keyboard::Key::Tab && appState != AppState::Vault)
            {
                std::vector<std::string> order = {"eticheta"};
                if (currentCat == Category::Accounts) order.insert(order.end(), {"user", "pass", "url", "note"});
                else if (currentCat == Category::Cards) order.insert(order.end(), {"holder", "num", "exp", "cvv"});
                else if (currentCat == Category::Identities)
                    order.insert(order.end(), {
                                     "fn", "ln", "tel", "em", "str", "ors",
                                     "jud", "tar", "cp"
                                 });
                else if (currentCat == Category::Notes) order.push_back("note");

                auto it = std::ranges::find(order, focusKey);
                focusKey = (it != order.end())
                               ? order[(std::distance(order.begin(), it) + 1) % order.size()]
                               : order[0];
                cursors[focusKey] = anchors[focusKey] = buffers[focusKey].getSize();
            }

            // === ENTER ===
            if (key->code == sf::Keyboard::Key::Enter)
            {
                if (appState == AppState::Vault) isSearchFocused = false;
                else triggerSave = true;
            }
        }
    }
}

/**
 * @brief Updates application logic (animations, background tasks).
 */
void UserInterface::update()
{
    if (genSliderDirty && !modalDraggingLen && genSliderClock.getElapsedTime().asSeconds() > 0.02f)
    {
        genOptions.length = pendingGenLength;
        regeneratePassword();
        genSliderDirty = false;
    }
}

/**
 * @brief Main render function. Clears screen, draws components, and displays.
 */
void UserInterface::render()
{
    window.clear(CLR_BG);

    if (showPassGenModal)
    {
        bool savedClick = click;
        click = false;
        drawSidebar();
        if (appState == AppState::Vault) drawVault(); else drawEditForm();
        click = savedClick;
        drawPasswordGeneratorModal();
        drawToast();
        click = false; // consume to avoid toggling underlying UI on the same frame
    }
    else
    {
        drawSidebar();
        if (appState == AppState::Vault) drawVault(); else drawEditForm();
        drawConfirmationModal();
        drawSortModal(); // [NOU] Aici se deseneaza fereastra de sortare
        drawPasswordGeneratorModal();
        drawToast();
    }
    window.display();
}

/**
 * @brief Draws the sidebar menu and logo. Handles category switching.
 */
void UserInterface::drawSidebar()
{
    // Sidebar Background
    sf::RectangleShape sb({SIDE_W, 900.f});
    sb.setFillColor(CLR_NAVY);
    window.draw(sb);

    // Logo & Title
    window.draw(logoSprite);
    window.draw(logoText);

    // Navigation Menu
    std::vector<std::pair<std::string, Category>> nav = {
        {"Accounts", Category::Accounts},
        {"Identities", Category::Identities},
        {"Credit cards", Category::Cards},
        {"Notes", Category::Notes},
        {"Security", Category::Security}
    };

    float ny = 140.f;
    for (auto& n : nav)
    {
        sf::RectangleShape b({SIDE_W, 50.f});
        b.setPosition({0, ny});
        bool isActive = (currentCat == n.second);
        bool isHover = (!showPassGenModal && !showSortModal) && b.getGlobalBounds().contains(mPos);
        b.setFillColor(isActive
                           ? sf::Color(20, 40, 70)
                           : (isHover ? sf::Color(15, 30, 55) : sf::Color::Transparent));

        // Click on Category
        if (!showSortModal && !showPassGenModal && click && b.getGlobalBounds().contains(mPos))
        {
            currentCat = n.second;
            appState = AppState::Vault;
            guiError = "";
            backToSecurity = false;
            showInlinePassGen = false;
            showBreachPanel = false;
            showPassGenModal = false;
            passGenModalArmed = false;
            modalDraggingLen = false;
            modalWasDraggingLen = false;
            hasLiveBreachInfo = false;

            // Resetare input forms
            cursors.clear();
            anchors.clear();
            showPass = false;
            showCVV = false;

            // Resetare Scroll
            scrollOffset = 0.f;

            // --- [NOU] RESET SEARCH LA SCHIMBAREA CATEGORIEI ---
            searchQuery = "";
            isSearchFocused = false;
            searchCursor = 0;
            searchAnchor = 0;
            // ---------------------------------------------------

            // Notify Security Monitor if entering Report view
            if (currentCat == Category::Security && GestionarParole::getInstance().getUserCrt())
            {
                securityNeedsRefresh = true;
            }
        }
        window.draw(b);

        sf::Text t = createUtf8Text(n.first, 18, isActive ? sf::Color::White : sf::Color(150, 160, 180));
        t.setPosition({50, ny + 12});
        window.draw(t);

        ny += 55.f;
    }
}

/**
 * @brief Draws the vault list and handles the Context Menu overlay.
 */
void UserInterface::drawVault()
{
    auto* user = GestionarParole::getInstance().getUserCrt();
    if (!user) return;

    // Security report has a custom layout.
    if (currentCat == Category::Security)
    {
        drawSecurityReport();
        return;
    }

    // ============================================================
    // 1. LOGICA DE DATE (FILTRARE & SORTARE)
    //    Calculam lista INAINTE de a desena orice, ca sa stim
    //    dimensiunile pentru scroll.
    // ============================================================

    std::vector<Seif*> visibleItems;

    // A. Filtrare
    for (size_t i = 0; i < user->getNrObiecte(); ++i)
    {
        Seif* itm = user->getObiectAt((int)i);

        // 1. Categorie
        bool matchCat = (currentCat == Category::Accounts && itm->getTip() == "Date Autentificare") ||
            (currentCat == Category::Cards && itm->getTip() == "Card Bancar") ||
            (currentCat == Category::Notes && itm->getTip() == "Notita Securizata") ||
            (currentCat == Category::Identities && itm->getTip() == "Identitate");
        if (!matchCat) continue;

        // 2. Search Avansat
        if (!searchQuery.empty())
        {
            std::string q = searchQuery;
            std::transform(q.begin(), q.end(), q.begin(), ::tolower);

            bool found = false;

            // a) Cautare in Eticheta
            std::string label = itm->getEticheta();
            std::transform(label.begin(), label.end(), label.begin(), ::tolower);
            if (label.find(q) != std::string::npos) found = true;

            // b) Cautare Extinsa (User, URL, Holder)
            if (!found)
            {
                if (const auto* da = dynamic_cast<const DateAutentificare*>(itm))
                {
                    std::string u = da->getUtilizator();
                    std::string url = da->getUrl();
                    std::transform(u.begin(), u.end(), u.begin(), ::tolower);
                    std::transform(url.begin(), url.end(), url.begin(), ::tolower);
                    if (u.find(q) != std::string::npos || url.find(q) != std::string::npos) found = true;
                }
                else if (const auto* cb = dynamic_cast<const CardBancar*>(itm))
                {
                    std::string holder = cb->getNumeDetinator();
                    std::transform(holder.begin(), holder.end(), holder.begin(), ::tolower);
                    if (holder.find(q) != std::string::npos) found = true;
                }
            }
            if (!found) continue;
        }
        visibleItems.push_back(itm);
    }

    // B. Sortare
    std::sort(visibleItems.begin(), visibleItems.end(), [&](Seif* a, Seif* b)
    {
        std::string s1, s2;

        if (currentCat == Category::Cards && activeCriteria == SortCriteria::CardHolder)
        {
            if (const auto* c1 = dynamic_cast<const CardBancar*>(a)) s1 = c1->getNumeDetinator();
            if (const auto* c2 = dynamic_cast<const CardBancar*>(b)) s2 = c2->getNumeDetinator();
        }
        else if (currentCat == Category::Cards && activeCriteria == SortCriteria::ExpiryDate)
        {
            if (const auto* c1 = dynamic_cast<const CardBancar*>(a)) s1 = c1->getDataExp();
            if (const auto* c2 = dynamic_cast<const CardBancar*>(b)) s2 = c2->getDataExp();
            if (s1.length() == 5) s1 = s1.substr(3, 2) + s1.substr(0, 2);
            if (s2.length() == 5) s2 = s2.substr(3, 2) + s2.substr(0, 2);
        }
        else
        {
            s1 = a->getEticheta();
            s2 = b->getEticheta();
        }

        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);

        if (activeSortOrder == SortOrder::Ascending) return s1 < s2;
        else return s1 > s2;
    });

    // Helper Load Lambda
    auto loadItemIntoBuffers = [&](Seif* item, bool focusPass = false)
    {
        activeItem = item;
        appState = AppState::Edit;
        buffers.clear();
        cursors.clear();
        anchors.clear();
        undoStack.clear();
        focusKey = focusPass ? "pass" : "eticheta";
        buffers["eticheta"] = toSfStr(item->getEticheta());

        if (const auto* da = dynamic_cast<const DateAutentificare*>(item))
        {
            buffers["user"] = toSfStr(da->getUtilizator());
            buffers["pass"] = toSfStr(da->getParola());
            buffers["url"] = toSfStr(da->getUrl());
            buffers["note"] = toSfStr(da->getNote());
        }
        else if (const auto* cb = dynamic_cast<const CardBancar*>(item))
        {
            buffers["num"] = toSfStr(cb->getNumar());
            buffers["exp"] = toSfStr(cb->getDataExp());
            buffers["cvv"] = toSfStr(cb->getCVV());
            buffers["holder"] = toSfStr(cb->getNumeDetinator());
        }
        else if (const auto* ns = dynamic_cast<const NotitaSecurizata*>(item))
        {
            buffers["note"] = toSfStr(ns->getNotita());
        }
        else if (const auto* id = dynamic_cast<const Identitate*>(item))
        {
            buffers["fn"] = toSfStr(id->get_prenume());
            buffers["ln"] = toSfStr(id->get_nume());
            buffers["tel"] = toSfStr(id->get_telefon());
            buffers["em"] = toSfStr(id->get_email());
            buffers["str"] = toSfStr(id->get_strada());
            buffers["ors"] = toSfStr(id->get_oras());
            buffers["jud"] = toSfStr(id->get_judet());
            buffers["tar"] = toSfStr(id->get_tara());
            buffers["cp"] = toSfStr(id->get_cod_postal());
        }
    };

    // Meniu Contextual (Calcul)
    sf::FloatRect menuBounds({0.f, 0.f}, {0.f, 0.f});
    struct MenuOpt
    {
        std::string txt;
        sf::Sprite& icon;
        int action;
    };
    std::vector<MenuOpt> opts;

    if (menuOpenItem != nullptr)
    {
        if (dynamic_cast<const DateAutentificare*>(menuOpenItem))
        {
            opts.push_back({"Copy Username", iconCopy, 10});
            opts.push_back({"Copy Password", iconCopy, 11});
            opts.push_back({"Open Website", iconGlobe, 2});
        }
        else if (dynamic_cast<const CardBancar*>(menuOpenItem))
        {
            opts.push_back({"Copy Number", iconCopy, 12});
            opts.push_back({"Copy CVV", iconCopy, 13});
        }
        else if (dynamic_cast<const NotitaSecurizata*>(menuOpenItem))
        {
            opts.push_back({"Copy Note", iconCopy, 14});
        }
        else if (dynamic_cast<const Identitate*>(menuOpenItem))
        {
            opts.push_back({"Copy Name", iconCopy, 20});
            opts.push_back({"Copy Phone", iconCopy, 21});
            opts.push_back({"Copy Email", iconCopy, 22});
            opts.push_back({"Copy Address", iconCopy, 23});
        }
        opts.push_back({"Edit Item", iconEdit, 3});
        opts.push_back({"Delete", iconTrash, 4});

        float menuH = opts.size() * 40.f + 10.f;
        menuBounds = sf::FloatRect({menuPos.x, menuPos.y}, {160.f, menuH});
    }
    bool mouseOverMenu = (menuOpenItem != nullptr && menuBounds.contains(mPos));

    // ============================================================
    // 2. DESENARE LISTA (Stratul de Jos)
    // ============================================================

    // Setam Y-ul de start putin mai jos, ca sa aiba loc header-ul
    float startY = 150.f;

    float totalContentHeight = visibleItems.size() * 85.f;
    float maxScroll = std::max(0.f, totalContentHeight - 650.f);
    if (scrollOffset < 0) scrollOffset = 0;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    float cy = startY - scrollOffset;
    bool clickedOnMenuTrigger = false;

    for (Seif* itm : visibleItems)
    {
        // Desenam doar daca e vizibil in zona de scroll
        if (cy > startY - 100.f && cy < 900.f)
        {
            sf::RectangleShape card({800.f, 75.f});
            card.setPosition({SIDE_W + 30, cy});

            // Hover
            if (card.getGlobalBounds().contains(mPos) && !mouseOverMenu && !showSortModal)
                card.setFillColor(sf::Color(250, 252, 255));
            else
                card.setFillColor(sf::Color::White);

            window.draw(card);

            // --- CALCUL TEXT SECUNDAR & TIP ---
            std::string sub = itm->getTip();
            bool isNote = false; // Flag pentru a sti daca centram titlul

            if (const auto* da = dynamic_cast<const DateAutentificare*>(itm))
            {
                sub = da->getUtilizator();
            }
            else if (const auto* cb = dynamic_cast<const CardBancar*>(itm))
            {
                std::string hiddenNum = "**** " + cb->getNumar().substr(
                    cb->getNumar().length() > 4 ? cb->getNumar().length() - 4 : 0);
                if (!cb->getNumeDetinator().empty()) sub = cb->getNumeDetinator() + " | " + hiddenNum;
                else sub = hiddenNum;
            }
            else if (const auto* id = dynamic_cast<const Identitate*>(itm))
            {
                sub = id->get_prenume() + " " + id->get_nume();
            }
            else if (dynamic_cast<const NotitaSecurizata*>(itm))
            {
                sub = ""; // Fara text secundar
                isNote = true; // Marcam ca fiind notita
            }

            // --- DESENARE TITLU ---
            sf::Text name = createUtf8Text(itm->getEticheta(), 20, sf::Color::Black);

            // [MODIFICAT] Daca e notita, centram vertical (25.f). Altfel punem sus (15.f).
            float titleY = isNote ? (cy + 25.f) : (cy + 15.f);
            name.setPosition({SIDE_W + 60, titleY});

            window.draw(name);

            // --- DESENARE TEXT SECUNDAR (Daca exista) ---
            if (!sub.empty())
            {
                sf::Text subT = createUtf8Text(sub, 15, sf::Color(150, 150, 160));
                subT.setPosition({SIDE_W + 60, cy + 42});
                window.draw(subT);
            }

            // Buton "..."
            iconMenu.setPosition({SIDE_W + 30 + 760, cy + 37});

            if (click && iconMenu.getGlobalBounds().contains(mPos) && !mouseOverMenu && !showSortModal)
            {
                menuOpenItem = itm;
                menuPos = {mPos.x - 140, mPos.y};
                clickedOnMenuTrigger = true;
            }
            window.draw(iconMenu);

            // Click pe Card
            if (click && card.getGlobalBounds().contains(mPos) && !clickedOnMenuTrigger && !mouseOverMenu && !
                showSortModal)
            {
                loadItemIntoBuffers(itm, false);
            }
        }
        cy += 85.f;
    }
    // ============================================================
    // 3. DESENARE HEADER (Stratul de Sus - FIXAT)
    //    Acum desenam fundalul header-ului PESTE lista scrolata
    // ============================================================

    // A. Masca Header (acopera cardurile care urca prea sus)
    // Dreptunghi de la Y=0 pana la startY (150)
    sf::RectangleShape headerBg({1200.f - SIDE_W, 150.f});
    headerBg.setPosition({SIDE_W, 0.f});
    headerBg.setFillColor(CLR_BG); // Aceeasi culoare cu fundalul ferestrei
    window.draw(headerBg);

    // B. Elementele Vizuale din Header
    std::string titleStr = "My Vault";
    if (currentCat == Category::Security) titleStr = "Security Analysis";
    sf::Text title = createUtf8Text(titleStr, 29, sf::Color::Black);
    title.setPosition({SIDE_W + 30, 30});
    window.draw(title);

    if (currentCat != Category::Security)
    {
        window.draw(searchBg);
        window.draw(icon_search);

        if (searchQuery.empty() && !isSearchFocused)
        {
            window.draw(searchPlaceholder);
        }
        else
        {
            // 1. Desenam SELECTION BOX (daca avem text selectat)
            if (isSearchFocused && searchCursor != searchAnchor)
            {
                size_t s = std::min(searchCursor, searchAnchor);
                size_t e = std::max(searchCursor, searchAnchor);

                // Calculam pozitiile vizuale
                // Nota: Folosim un text temporar pentru a masura
                sf::Text tempText = searchText;
                tempText.setString(searchQuery);

                float startX = tempText.findCharacterPos(s).x;
                float endX = tempText.findCharacterPos(e).x;

                sf::RectangleShape selRect({endX - startX, 20.f});
                selRect.setPosition({startX, searchBg.getPosition().y + 10.f});
                selRect.setFillColor(CLR_SELECT); // Albastru deschis
                window.draw(selRect);
            }

            // 2. Desenam Textul
            searchText.setString(searchQuery);
            window.draw(searchText);

            // 3. Desenam CURSORUL (daca avem focus)
            if (isSearchFocused && showCursor)
            {
                sf::Vector2f charPos = searchText.findCharacterPos(searchCursor);
                sf::RectangleShape cursorLine({1.f, 20.f});
                cursorLine.setFillColor(sf::Color::Black);
                cursorLine.setPosition({charPos.x, searchBg.getPosition().y + 10.f});
                window.draw(cursorLine);
            }
        }

        window.draw(sortBtn);
        window.draw(icon_sort);
    }
    // ============================================================
    // 4. DESENARE FAB & MENIU (Stratul Cel Mai de Sus)
    // ============================================================

    // --- FAB (+ Add) ---
    {
        sf::RectangleShape fab({160.f, 45.f});
        fab.setPosition({1000, 810});
        fab.setFillColor(CLR_ACCENT);
        if (fab.getGlobalBounds().contains(mPos) && !mouseOverMenu && !showSortModal)
            fab.setFillColor(
                sf::Color(0, 80, 200));

        if (((click && fab.getGlobalBounds().contains(mPos) && !mouseOverMenu && !showSortModal)) || triggerSave)
        {
            appState = AppState::Add;
            activeItem = nullptr;
            buffers.clear();
            cursors.clear();
            anchors.clear();
            undoStack.clear();
            focusKey = "eticheta";
            triggerSave = false;
            menuOpenItem = nullptr;
        }
        window.draw(fab);
        sf::Text fabT = createUtf8Text("+ Add item", 20, sf::Color::White);
        fabT.setPosition({1000 + 80.f - fabT.getGlobalBounds().size.x / 2.f, 810 + 10.f});
        window.draw(fabT);
    }

    // --- MENIU CONTEXTUAL (Overlay) ---
    if (menuOpenItem != nullptr)
    {
        sf::RectangleShape menuBg({160.f, menuBounds.size.y});
        menuBg.setPosition(menuPos);
        menuBg.setFillColor(sf::Color::White);
        menuBg.setOutlineThickness(1);
        menuBg.setOutlineColor(sf::Color(200, 200, 200));
        sf::RectangleShape shadow({160.f, menuBounds.size.y});
        shadow.setPosition({menuPos.x + 3, menuPos.y + 3});
        shadow.setFillColor(sf::Color(0, 0, 0, 30));
        window.draw(shadow);
        window.draw(menuBg);

        float my = menuPos.y + 5;
        bool itemClicked = false;
        for (auto& opt : opts)
        {
            sf::RectangleShape row({150.f, 35.f});
            row.setPosition({menuPos.x + 5, my});
            bool hover = row.getGlobalBounds().contains(mPos);
            row.setFillColor(hover ? sf::Color(240, 245, 255) : sf::Color::Transparent);
            window.draw(row);
            opt.icon.setPosition({menuPos.x + 20, my + 17});
            opt.icon.setColor(opt.action == 4 ? CLR_ERR : sf::Color(80, 80, 90));
            window.draw(opt.icon);
            sf::Text tOpt = createUtf8Text(opt.txt, 14, opt.action == 4 ? CLR_ERR : sf::Color::Black);
            tOpt.setPosition({menuPos.x + 45, my + 8});
            window.draw(tOpt);

            if (click && hover && !showSortModal)
            {
                itemClicked = true;

                if (opt.action == 20)
                {
                    if (const auto* id = dynamic_cast<const Identitate*>(menuOpenItem))
                        sf::Clipboard::setString(
                            id->get_prenume() + " " + id->get_nume());
                    showToast("Name copied!");
                }
                if (opt.action == 21)
                {
                    if (const auto* id = dynamic_cast<const Identitate*>(menuOpenItem)) sf::Clipboard::setString(id->get_telefon());
                    showToast("Phone copied!");
                }
                if (opt.action == 22)
                {
                    if (const auto* id = dynamic_cast<const Identitate*>(menuOpenItem)) sf::Clipboard::setString(id->get_email());
                    showToast("Email copied!");
                }
                if (opt.action == 23)
                {
                    if (const auto* id = dynamic_cast<const Identitate*>(menuOpenItem))
                        sf::Clipboard::setString(
                            id->get_strada() + ", " + id->get_oras() + ", " + id->get_judet());
                    showToast("Address copied!");
                }
                if (opt.action == 10)
                {
                    if (const auto* da = dynamic_cast<const DateAutentificare*>(menuOpenItem))
                        sf::Clipboard::setString(
                            da->getUtilizator());
                    showToast("Copied Username!");
                }
                if (opt.action == 11)
                {
                    if (const auto* da = dynamic_cast<const DateAutentificare*>(menuOpenItem))
                        sf::Clipboard::setString(
                            da->getParola());
                    showToast("Copied Password!");
                }
                if (opt.action == 12)
                {
                    if (const auto* cb = dynamic_cast<const CardBancar*>(menuOpenItem)) sf::Clipboard::setString(cb->getNumar());
                    showToast("Copied Card Number!");
                }
                if (opt.action == 13)
                {
                    if (const auto* cb = dynamic_cast<const CardBancar*>(menuOpenItem)) sf::Clipboard::setString(cb->getCVV());
                    showToast("Copied CVV!");
                }
                if (opt.action == 14)
                {
                    if (const auto* ns = dynamic_cast<const NotitaSecurizata*>(menuOpenItem))
                        sf::Clipboard::setString(
                            ns->getNotita());
                    showToast("Copied Note!");
                }

                if (opt.action == 2)
                {
                    if (const auto* da = dynamic_cast<const DateAutentificare*>(menuOpenItem))
                    {
#ifdef _WIN32
                        ShellExecuteA(0, "open", da->getUrl().c_str(), 0, 0, SW_SHOW);
#else
                        std::string c = "xdg-open " + da->getUrl(); system(c.c_str());
#endif
                    }
                }
                if (opt.action == 3) loadItemIntoBuffers(menuOpenItem);
                if (opt.action == 4)
                {
                    itemPendingDelete = menuOpenItem;
                    showDeleteConfirm = true;
                }

                // RESET SEARCH
                searchQuery = "";
                isSearchFocused = false;

                menuOpenItem = nullptr;
            }
            my += 40;
        }
        if (click && !itemClicked && !clickedOnMenuTrigger && !menuBg.getGlobalBounds().contains(mPos))
            menuOpenItem = nullptr;
    }
}

/**
 * @brief Draws the edit/add form for items. Handles save logic.
 */
void UserInterface::drawSecurityReport()
{
    auto* user = GestionarParole::getInstance().getUserCrt();
    if (!user || monitor == nullptr)
    {
        sf::Text msg = createUtf8Text("No user logged in.", 18, sf::Color::Black);
        msg.setPosition({SIDE_W + 40, 200});
        window.draw(msg);
        return;
    }

    // Actualizam alertele doar la cerere pentru a evita lag-ul
    if (securityNeedsRefresh && !showPassGenModal)
    {
        monitor->update();
        securityNeedsRefresh = false;
    }

    const auto& alerts = monitor->getAlertDetails();
    struct ItemIssue { std::string label; std::string msg; };
    std::vector<ItemIssue> dupItems;
    std::vector<ItemIssue> weakItems;
    std::vector<ItemIssue> breachItems;
    auto trim = [](std::string s)
    {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
        return s;
    };
    for (const auto& a : alerts)
    {
        if (a.mesaj.find("Password reused across") != std::string::npos)
        {
            auto pos = a.mesaj.find(':');
            if (pos != std::string::npos)
            {
                std::string labels = a.mesaj.substr(pos + 1);
                std::stringstream ss(labels);
                std::string token;
                while (std::getline(ss, token, ','))
                {
                    token = trim(token);
                    if (!token.empty()) dupItems.push_back({token, "Duplicate password"});
                }
            }
        }
        if (a.mesaj.find("weak") != std::string::npos || a.mesaj.find("below policy length") != std::string::npos
            || a.mesaj.find("misses uppercase") != std::string::npos)
        {
            auto p1 = a.mesaj.find('\'');
            auto p2 = a.mesaj.find('\'', p1 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1 + 1)
            {
                std::string label = a.mesaj.substr(p1 + 1, p2 - p1 - 1);
                weakItems.push_back({label, a.mesaj});
            }
        }
        if (a.mesaj.find("found in breaches") != std::string::npos)
        {
            auto p1 = a.mesaj.find('\'');
            auto p2 = a.mesaj.find('\'', p1 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1 + 1)
            {
                std::string label = a.mesaj.substr(p1 + 1, p2 - p1 - 1);
                breachItems.push_back({label, a.mesaj});
            }
        }
    }
    auto isGroupedMsg = [&](const std::string& msg)
    {
        return msg.find("Password reused across") != std::string::npos ||
               msg.find("weak") != std::string::npos ||
               msg.find("below policy length") != std::string::npos ||
               msg.find("misses uppercase") != std::string::npos ||
               msg.find("found in breaches") != std::string::npos;
    };

    float startY = 170.f;

    auto openItemForLabel = [&](const std::string& label)
    {
        if (!user) return;
        for (size_t i = 0; i < user->getNrObiecte(); ++i)
        {
            Seif* itm = user->getObiectAt(static_cast<int>(i));
            if (itm && itm->getEticheta() == label)
            {
                activeItem = itm;
                appState = AppState::Edit;
                buffers.clear();
                cursors.clear();
                anchors.clear();
                undoStack.clear();
                focusKey = "eticheta";
                buffers["eticheta"] = toSfStr(itm->getEticheta());

                if (const auto* da = dynamic_cast<const DateAutentificare*>(itm))
                {
                    currentCat = Category::Accounts;
                    buffers["user"] = toSfStr(da->getUtilizator());
                    buffers["pass"] = toSfStr(da->getParola());
                    buffers["url"] = toSfStr(da->getUrl());
                    buffers["note"] = toSfStr(da->getNote());
                    focusKey = "pass"; // focus password when coming from Security
                }
                else if (const auto* cb = dynamic_cast<const CardBancar*>(itm))
                {
                    currentCat = Category::Cards;
                    buffers["num"] = toSfStr(cb->getNumar());
                    buffers["exp"] = toSfStr(cb->getDataExp());
                    buffers["cvv"] = toSfStr(cb->getCVV());
                    buffers["holder"] = toSfStr(cb->getNumeDetinator());
                }
                else if (const auto* ns = dynamic_cast<const NotitaSecurizata*>(itm))
                {
                    currentCat = Category::Notes;
                    buffers["note"] = toSfStr(ns->getNotita());
                }
                else if (const auto* id = dynamic_cast<const Identitate*>(itm))
                {
                    currentCat = Category::Identities;
                    buffers["fn"] = toSfStr(id->get_prenume());
                    buffers["ln"] = toSfStr(id->get_nume());
                    buffers["tel"] = toSfStr(id->get_telefon());
                    buffers["em"] = toSfStr(id->get_email());
                    buffers["str"] = toSfStr(id->get_strada());
                    buffers["ors"] = toSfStr(id->get_oras());
                    buffers["jud"] = toSfStr(id->get_judet());
                    buffers["tar"] = toSfStr(id->get_tara());
                    buffers["cp"] = toSfStr(id->get_cod_postal());
                }
                backToSecurity = true;
                return;
            }
        }
    };

    // Duplicate summary line
    {
        sf::RectangleShape dupBox({800.f, 45.f});
        dupBox.setPosition({SIDE_W + 30, startY});
        dupBox.setFillColor(sf::Color(250, 250, 252));
        dupBox.setOutlineThickness(1);
        dupBox.setOutlineColor(sf::Color(220, 220, 220));
        window.draw(dupBox);

        std::string dupText = "Duplicate passwords: " + std::to_string(dupItems.size()) + " item(s)";
        sf::Text dupT = createUtf8Text(dupText, 18, !dupItems.empty() ? CLR_ERR : sf::Color(40, 120, 70));
        dupT.setStyle(sf::Text::Bold);
        dupT.setPosition({dupBox.getPosition().x + 12.f, dupBox.getPosition().y + 10.f});
        window.draw(dupT);

        // Chevron
        sf::Sprite caret = icon_sort;
        caret.setColor(sf::Color(90, 90, 100));
        caret.setScale({0.8f, showDupPanel ? -0.8f : 0.8f});
        sf::FloatRect cb = caret.getLocalBounds();
        caret.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
        caret.setPosition({dupBox.getPosition().x + dupBox.getSize().x - 18.f, dupBox.getPosition().y + 22.f});
        window.draw(caret);

        bool overDup = (!showPassGenModal && !showSortModal) && dupBox.getGlobalBounds().contains(mPos);
        if (overDup && click)
        {
            showDupPanel = !showDupPanel;
            scrollOffset = 0.f;
        }

        startY += dupBox.getSize().y;

        if (showDupPanel && !dupItems.empty())
        {
            for (const auto& entry : dupItems)
            {
                sf::RectangleShape row({800.f, 32.f});
                row.setPosition({SIDE_W + 30, startY + 5.f});
                row.setFillColor(sf::Color(252, 252, 252));
                row.setOutlineThickness(1);
                row.setOutlineColor(sf::Color(230, 230, 230));
                bool hover = (!showPassGenModal && !showSortModal) && row.getGlobalBounds().contains(mPos);
                if (hover) row.setFillColor(sf::Color(240, 245, 255));
                window.draw(row);

                sf::Text t = createUtf8Text(entry.label + " — duplicate password", 15, sf::Color::Black);
                t.setPosition({row.getPosition().x + 10.f, row.getPosition().y + 6.f});
                window.draw(t);

                if (hover && click) openItemForLabel(entry.label);

                startY += row.getSize().y + 5.f;
            }
        }

        startY += 10.f;
    }

    // Breached passwords summary line
    {
        sf::RectangleShape breachBox({800.f, 45.f});
        breachBox.setPosition({SIDE_W + 30, startY});
        breachBox.setFillColor(sf::Color(250, 250, 252));
        breachBox.setOutlineThickness(1);
        breachBox.setOutlineColor(sf::Color(220, 220, 220));
        window.draw(breachBox);

        std::string breachText = "Passwords found in breaches: " + std::to_string(breachItems.size()) + " item(s)";
        sf::Text breachT = createUtf8Text(breachText, 18, !breachItems.empty() ? CLR_ERR : sf::Color(40, 120, 70));
        breachT.setStyle(sf::Text::Bold);
        breachT.setPosition({breachBox.getPosition().x + 12.f, breachBox.getPosition().y + 10.f});
        window.draw(breachT);

        sf::Sprite caret = icon_sort;
        caret.setColor(sf::Color(90, 90, 100));
        caret.setScale({0.8f, showSecurityGenPanel ? -0.8f : 0.8f});
        sf::FloatRect cb = caret.getLocalBounds();
        caret.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
        caret.setPosition({breachBox.getPosition().x + breachBox.getSize().x - 18.f, breachBox.getPosition().y + 22.f});
        window.draw(caret);

        bool overBreach = (!showPassGenModal && !showSortModal) && breachBox.getGlobalBounds().contains(mPos);
        if (overBreach && click)
        {
            showSecurityGenPanel = !showSecurityGenPanel;
            scrollOffset = 0.f;
        }

        startY += breachBox.getSize().y;

        if (showSecurityGenPanel && !breachItems.empty())
        {
            for (const auto& entry : breachItems)
            {
                sf::RectangleShape row({800.f, 32.f});
                row.setPosition({SIDE_W + 30, startY + 5.f});
                row.setFillColor(sf::Color(252, 252, 252));
                row.setOutlineThickness(1);
                row.setOutlineColor(sf::Color(230, 230, 230));
                bool hover = (!showPassGenModal && !showSortModal) && row.getGlobalBounds().contains(mPos);
                if (hover) row.setFillColor(sf::Color(240, 245, 255));
                window.draw(row);

                sf::Text t = createUtf8Text(entry.label + " — " + entry.msg, 15, sf::Color::Black);
                t.setPosition({row.getPosition().x + 10.f, row.getPosition().y + 6.f});
                window.draw(t);

                if (hover && click) openItemForLabel(entry.label);

                startY += row.getSize().y + 5.f;
            }
        }

        startY += 10.f;
    }

    // Weak passwords summary line
    int weakCount = static_cast<int>(weakItems.size());
    {
        sf::RectangleShape weakBox({800.f, 45.f});
        weakBox.setPosition({SIDE_W + 30, startY});
        weakBox.setFillColor(sf::Color(250, 250, 252));
        weakBox.setOutlineThickness(1);
        weakBox.setOutlineColor(sf::Color(220, 220, 220));
        window.draw(weakBox);

        std::string weakText = "Weak passwords: " + std::to_string(weakCount) + " item(s)";
        sf::Text weakT = createUtf8Text(weakText, 18, weakCount > 0 ? sf::Color(200, 140, 30) : sf::Color(40, 120, 70));
        weakT.setStyle(sf::Text::Bold);
        weakT.setPosition({weakBox.getPosition().x + 12.f, weakBox.getPosition().y + 10.f});
        window.draw(weakT);

        sf::Sprite caret = icon_sort;
        caret.setColor(sf::Color(90, 90, 100));
        caret.setScale({0.8f, showWeakPanel ? -0.8f : 0.8f});
        sf::FloatRect cb = caret.getLocalBounds();
        caret.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
        caret.setPosition({weakBox.getPosition().x + weakBox.getSize().x - 18.f, weakBox.getPosition().y + 22.f});
        window.draw(caret);

        bool overWeak = (!showPassGenModal && !showSortModal) && weakBox.getGlobalBounds().contains(mPos);
        if (overWeak && click)
        {
            showWeakPanel = !showWeakPanel;
            scrollOffset = 0.f;
        }

        startY += weakBox.getSize().y;

        if (showWeakPanel && weakCount > 0)
        {
            for (const auto& entry : weakItems)
            {
                sf::RectangleShape row({800.f, 32.f});
                row.setPosition({SIDE_W + 30, startY + 5.f});
                row.setFillColor(sf::Color(252, 252, 252));
                row.setOutlineThickness(1);
                row.setOutlineColor(sf::Color(230, 230, 230));
                bool hover = (!showPassGenModal && !showSortModal) && row.getGlobalBounds().contains(mPos);
                if (hover) row.setFillColor(sf::Color(240, 245, 255));
                window.draw(row);

                sf::Text t = createUtf8Text(entry.label + " — " + entry.msg, 15, sf::Color::Black);
                t.setPosition({row.getPosition().x + 10.f, row.getPosition().y + 6.f});
                window.draw(t);

                if (hover && click) openItemForLabel(entry.label);

                startY += row.getSize().y + 5.f;
            }
        }

        startY += 10.f;
    }

    // After the fixed sections, set the content start for scrollable alerts
    float contentStartY = startY + 5.f; // small padding below sections
    float cardW = 800.f;
    float cardH = 60.f;
    float gap = 12.f;
    float viewHeight = std::max(80.f, static_cast<float>(window.getSize().y) - contentStartY - 40.f);

    // Scroll bounds
    size_t visibleCount = 0;
    for (const auto& al : alerts) if (!isGroupedMsg(al.mesaj)) ++visibleCount;

    float totalContentHeight = static_cast<float>(visibleCount) * (cardH + gap);
    float maxScroll = std::max(0.f, totalContentHeight - viewHeight);
    if (scrollOffset < 0.f) scrollOffset = 0.f;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    if (alerts.empty())
    {
        sf::RectangleShape okCard({cardW, cardH});
        okCard.setPosition({SIDE_W + 30, startY});
        okCard.setFillColor(sf::Color(230, 245, 234));
        okCard.setOutlineThickness(1);
        okCard.setOutlineColor(sf::Color(180, 210, 185));
        window.draw(okCard);

        sf::Text msg = createUtf8Text("No security issues detected.", 18, sf::Color(30, 120, 70));
        msg.setPosition({SIDE_W + 50, startY + 20});
        window.draw(msg);
        return;
    }

    auto colorForSeverity = [](SecurityMonitor::Severity s) -> sf::Color
    {
        if (s == SecurityMonitor::Severity::Critical) return sf::Color(220, 53, 69);      // red
        if (s == SecurityMonitor::Severity::Warning) return sf::Color(245, 180, 60);      // amber
        return sf::Color(90, 120, 160);                                                   // info blue-gray
    };

    size_t drawIdx = 0;

    // Render into an offscreen texture to hard-clip the scrollable area
    sf::RenderTexture rt({static_cast<unsigned>(cardW), static_cast<unsigned>(viewHeight)});
    rt.clear(sf::Color::Transparent);

    for (size_t i = 0; i < alerts.size(); ++i)
    {
        const auto& al = alerts[i];
        if (isGroupedMsg(al.mesaj)) continue;
        float y = drawIdx * (cardH + gap) - scrollOffset;
        ++drawIdx;

        if (y + cardH < 0.f || y > viewHeight) continue;

        sf::RectangleShape card({cardW, cardH});
        card.setPosition({0.f, y});
        card.setFillColor(sf::Color(250, 250, 250));
        card.setOutlineThickness(1);
        card.setOutlineColor(sf::Color(220, 220, 220));
        rt.draw(card);

        sf::Color accent = colorForSeverity(al.severity);

        sf::RectangleShape dot({6.f, 32.f});
        dot.setFillColor(accent);
        dot.setPosition({10.f, y + 14});
        rt.draw(dot);

        sf::Text txt = createUtf8Text(al.mesaj, 16, sf::Color::Black);
        txt.setPosition({26.f, y + 18});
        rt.draw(txt);
    }

    rt.display();
    sf::Sprite listSprite(rt.getTexture());
    listSprite.setPosition({SIDE_W + 30.f, contentStartY});
    window.draw(listSprite);

    // Redraw header and buttons on top (to mask scrolling content)
    sf::RectangleShape headerBg({1200.f - SIDE_W, 150.f});
    headerBg.setPosition({SIDE_W, 0.f});
    headerBg.setFillColor(CLR_BG);
    window.draw(headerBg);

    sf::Text title = createUtf8Text("Security report", 26, sf::Color::Black);
    title.setPosition({SIDE_W + 30, 30});
    window.draw(title);

    // Buttons: Refresh & Password Generator
    auto drawSmallBtn = [&](const std::string& label, float x, float y, sf::Color bg, auto onClick)
    {
        sf::RectangleShape btn({170.f, 38.f});
        btn.setPosition({x, y});
        btn.setFillColor(bg);
        btn.setOutlineThickness(1.2f);
        btn.setOutlineColor(sf::Color(200, 200, 200));
        if (btn.getGlobalBounds().contains(mPos))
        {
            int r = std::max(0, static_cast<int>(bg.r) - 10);
            int g = std::max(0, static_cast<int>(bg.g) - 10);
            int b = std::max(0, static_cast<int>(bg.b) - 10);
            btn.setFillColor(sf::Color(static_cast<unsigned char>(r),
                                       static_cast<unsigned char>(g),
                                       static_cast<unsigned char>(b),
                                       bg.a));
            if (click) onClick();
        }
        window.draw(btn);
        sf::Text t = createUtf8Text(label, 16, sf::Color::White);
        t.setPosition({x + 10.f, y + 8.f});
        window.draw(t);
    };

    drawSmallBtn("Refresh report", 700.f, 35.f, sf::Color(80, 80, 90), [&]()
    {
        if (!showPassGenModal)
        {
            securityNeedsRefresh = true;
        }
    });

    drawSmallBtn("Password Generator", 900.f, 35.f, sf::Color(80, 80, 90), [&]()
    {
        generatorTargetKey.clear(); // from menu, not autofilling a field
        genOptions.length = DEFAULT_GEN_LEN;
        pendingGenLength = DEFAULT_GEN_LEN;
        genSliderDirty = false;
        regeneratePassword();
        showPassGenModal = true;
        passGenModalArmed = false;
        modalDraggingLen = false;
        modalWasDraggingLen = false;
    });
}

/**
 * @brief Draws the edit/add form for items. Handles save logic.
 */
void UserInterface::drawEditForm()
{
    std::vector<std::pair<std::string, std::string>> fields = {{"Title", "eticheta"}};
    if (currentCat == Category::Accounts)
        fields.insert(fields.end(), {
                          {"Username", "user"}, {"Password", "pass"}, {"Website URL", "url"}, {"Notes", "note"}
                      });
    else if (currentCat == Category::Cards)
        fields.insert(fields.end(), {
                          {"Cardholder Name", "holder"}, {"Card Number", "num"}, {"Expiry Date", "exp"}, {"CVV", "cvv"}
                      });
    else if (currentCat == Category::Identities)
        fields.insert(fields.end(), {
                          {"First Name", "fn"}, {"Last Name", "ln"}, {"Phone", "tel"}, {"Email", "em"},
                          {"Street", "str"}, {"City", "ors"}, {"County", "jud"}, {"Country", "tar"},
                          {"Postal Code", "cp"}
                      });
    else if (currentCat == Category::Notes)
        fields.push_back({"Note Content", "note"});

    float fy = 50.f;
    float step = (fields.size() > 5) ? 65.f : 90.f;

    for (auto& f : fields)
    {
        float extraAfterField = 0.f;

        // 1. Eticheta campului
        sf::Text lab = createUtf8Text(f.first, 16, sf::Color(100, 100, 100));
        lab.setPosition({SIDE_W + 40, fy});
        window.draw(lab);

        // 2. Cutia de input
        sf::RectangleShape bx({600.f, 40.f});
        bx.setPosition({SIDE_W + 40, fy + 25});
        bx.setFillColor(sf::Color::White);
        bx.setOutlineThickness(focusKey == f.second ? 2.f : 1.f);
        bx.setOutlineColor(focusKey == f.second ? CLR_ACCENT : sf::Color(200, 200, 200));
        window.draw(bx);

        // 3. Procesare text (Masking cu Bullet)
        sf::String disp = buffers[f.second];
        bool isHidden = false;

        // Logica pentru PAROLA
        if (f.second == "pass")
        {
            if (!showPass) isHidden = true;
        }
        // Logica pentru CVV (si PIN daca ar exista)
        else if (f.second == "cvv")
        {
            if (!showCVV) isHidden = true;
        }

        // Aplicare masca Bullet (•)
        if (isHidden)
        {
            // Construim un string de bullets de aceeasi lungime
            std::string bullets;
            for (size_t i = 0; i < disp.getSize(); ++i) bullets += "\xE2\x80\xA2"; // Cod UTF-8 pentru Bullet
            disp = sf::String::fromUtf8(bullets.begin(), bullets.end());
        }

        sf::Text txt(font, disp, 19);
        txt.setFillColor(sf::Color::Black);

        // 4. Pozitionare Text (Centrare verticala pentru Bullets)
        float textX = SIDE_W + 50.f;
        float textY = fy + 31.f; // Pozitia standard

        if (isHidden && disp.getSize() > 0)
        {
            sf::FloatRect bounds = txt.getLocalBounds();
            float boxCenterY = fy + 25.f + 20.f;
            // SFML 3.0 Centering Fix
            txt.setOrigin(sf::Vector2f(0.f, bounds.position.y + bounds.size.y / 2.0f));
            textY = boxCenterY;
        }
        txt.setPosition({textX, textY});

        // Click focus
        if (click && bx.getGlobalBounds().contains(mPos))
        {
            focusKey = f.second;
            cursors[f.second] = anchors[f.second] = getIndexAtMouse(txt, mPos.x);
            if (dblClick)
            {
                anchors[f.second] = 0;
                cursors[f.second] = buffers[f.second].getSize();
            }
        }

        // Desenare Selectie & Cursor
        if (focusKey == f.second && anchors[f.second] != cursors[f.second])
        {
            size_t s = std::min(anchors[f.second], cursors[f.second]);
            size_t e = std::max(anchors[f.second], cursors[f.second]);
            float startX = txt.findCharacterPos(s).x;
            float endX = txt.findCharacterPos(e).x;
            sf::RectangleShape sel({endX - startX, 24.f});
            sel.setPosition({startX, fy + 28});
            sel.setFillColor(CLR_SELECT);
            window.draw(sel);
        }
        window.draw(txt);
        if (focusKey == f.second && showCursor)
        {
            sf::RectangleShape cl({2.f, 24.f});
            cl.setPosition({txt.findCharacterPos(cursors[f.second]).x, fy + 28});
            cl.setFillColor(CLR_ACCENT);
            window.draw(cl);
        }

        // ============================================================
        //                   BUTOANE DIN INTERIORUL CAMPULUI
        // ============================================================

        float btnX = SIDE_W + 40 + 600 - 40; // Pozitia butonului cel mai din dreapta
        float btnY = fy + 30;
        float currentX = btnX;

        // --- A. BUTON COPY ---
        // Apare la 'pass', 'cvv', 'num' (card number), si optional 'user'
        bool hasCopy = (f.second == "pass" || f.second == "cvv" || f.second == "num" || f.second == "user");

        if (hasCopy)
        {
            sf::RectangleShape copyBtn({30.f, 30.f});
            copyBtn.setPosition({currentX, btnY});
            copyBtn.setFillColor(sf::Color(240, 240, 245));
            copyBtn.setOutlineThickness(1);
            copyBtn.setOutlineColor(sf::Color(200, 200, 200));

            if (copyBtn.getGlobalBounds().contains(mPos))
            {
                copyBtn.setFillColor(sf::Color(220, 225, 255));
                copyBtn.setOutlineColor(CLR_ACCENT);
                if (click)
                {
                    std::string rawVal = u8ToStr(buffers[f.second].toUtf8());
                    sf::Clipboard::setString(rawVal);
                    showToast("Copied to clipboard!");
                }
            }
            window.draw(copyBtn);

            // Iconita Copy (reutilizam sprite-ul existent)
            iconCopy.setPosition({copyBtn.getPosition().x + 15, copyBtn.getPosition().y + 15});
            iconCopy.setColor(sf::Color(50, 50, 60));
            window.draw(iconCopy);

            currentX -= 35.f;
        }

        // --- B. BUTON SHOW/HIDE (Ochi) ---
        // Apare doar la 'pass' si 'cvv'
        bool hasEye = (f.second == "pass" || f.second == "cvv");
        if (hasEye)
        {
            sf::RectangleShape eyeBtn({30.f, 30.f});
            eyeBtn.setPosition({currentX, btnY});
            eyeBtn.setFillColor(sf::Color(240, 240, 245));
            eyeBtn.setOutlineThickness(1);
            eyeBtn.setOutlineColor(sf::Color(200, 200, 200));

            if (eyeBtn.getGlobalBounds().contains(mPos))
            {
                eyeBtn.setFillColor(sf::Color(220, 225, 255));
                eyeBtn.setOutlineColor(CLR_ACCENT);
                if (click)
                {
                    if (f.second == "pass") showPass = !showPass;
                    if (f.second == "cvv") showCVV = !showCVV;
                }
            }
            window.draw(eyeBtn);

            // Desenam iconita corecta
            bool currentState = (f.second == "pass") ? showPass : showCVV;
            sf::Sprite& icon = currentState ? iconEyeClosed : iconEyeOpen; // Daca e vizibil -> Iconita "Inchide"

            icon.setPosition({eyeBtn.getPosition().x + 15, eyeBtn.getPosition().y + 15});
            icon.setColor(sf::Color(50, 50, 60));
            window.draw(icon);

            currentX -= 35.f;
        }

        // --- C. BUTON GO TO URL ---
        if (f.second == "url")
        {
            sf::RectangleShape goBtn({30.f, 30.f});
            goBtn.setPosition({btnX, btnY});
            goBtn.setFillColor(sf::Color(240, 240, 245));
            goBtn.setOutlineThickness(1);
            goBtn.setOutlineColor(sf::Color(200, 200, 200));

            if (goBtn.getGlobalBounds().contains(mPos))
            {
                goBtn.setFillColor(sf::Color(220, 225, 255));
                goBtn.setOutlineColor(CLR_ACCENT);
                if (click)
                {
                    std::string rawUrl = u8ToStr(buffers["url"].toUtf8());
                    if (!rawUrl.empty())
                    {
                        if (rawUrl.find("http") == std::string::npos) rawUrl = "https://" + rawUrl;
#ifdef _WIN32
                        ShellExecuteA(0, "open", rawUrl.c_str(), 0, 0, SW_SHOW);
#else
                        std::string cmd = "xdg-open " + rawUrl; system(cmd.c_str());
#endif
                    }
                }
            }
            window.draw(goBtn);
            iconGlobe.setPosition({goBtn.getPosition().x + 15, goBtn.getPosition().y + 15});
            iconGlobe.setColor(sf::Color(50, 50, 60));
            window.draw(iconGlobe);
        }

        // --- D. Inline Password Generator toggle & panel (only for Accounts, password field) ---
        if (currentCat == Category::Accounts && f.second == "pass")
        {
            float toggleY = fy + 75.f;
            sf::RectangleShape tg({600.f, 35.f});
            tg.setPosition({SIDE_W + 40, toggleY});
            tg.setFillColor(sf::Color(245, 247, 250));
            tg.setOutlineThickness(1);
            tg.setOutlineColor(sf::Color(210, 210, 210));
            bool hover = tg.getGlobalBounds().contains(mPos);
            if (hover) tg.setFillColor(sf::Color(235, 240, 250));
            window.draw(tg);

            sf::Text tgl = createUtf8Text("Generate password", 15, sf::Color(70, 70, 80));
            tgl.setPosition({tg.getPosition().x + 12, tg.getPosition().y + 8});
            window.draw(tgl);

            // Small caret using sort icon
            sf::Sprite caret = icon_sort;
            caret.setColor(sf::Color(90, 90, 100));
            caret.setScale({0.8f, showInlinePassGen ? -0.8f : 0.8f});
            sf::FloatRect cb = caret.getLocalBounds();
            caret.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
            caret.setPosition({tg.getPosition().x + tg.getSize().x - 18.f, tg.getPosition().y + 17.f});
            window.draw(caret);

            if (click && hover)
            {
                showInlinePassGen = !showInlinePassGen;
                if (showInlinePassGen)
                {
                    pendingGenLength = genOptions.length;
                    genSliderDirty = false;
                    regeneratePassword();
                }
            }

            float panelHeight = 0.f;
            if (showInlinePassGen)
            {
                float panelY = toggleY + 40.f;

                // Panel background
                sf::RectangleShape panel({600.f, 190.f});
                panel.setPosition({SIDE_W + 40, panelY});
                panel.setFillColor(sf::Color::White);
                panel.setOutlineThickness(1);
                panel.setOutlineColor(sf::Color(210, 210, 210));
                window.draw(panel);

                // Generated password row
                sf::RectangleShape pwdBox({panel.getSize().x - 30.f, 36.f});
                pwdBox.setPosition({panel.getPosition().x + 10.f, panelY + 10.f});
                pwdBox.setFillColor(sf::Color(248, 248, 250));
                pwdBox.setOutlineThickness(1);
                pwdBox.setOutlineColor(sf::Color(220, 220, 220));
                window.draw(pwdBox);

            auto drawColoredPwd = [&](float x, float y, unsigned int sz)
            {
                float cx = x;
                for (size_t i = 0; i < generatedPass.size(); ++i)
                {
                    sf::Text ch = createUtf8Text(std::string(1, generatedPass[i]), sz,
                                                 i < generatedColors.size() ? generatedColors[i] : sf::Color::Black);
                    ch.setPosition({cx, y});
                    window.draw(ch);
                    cx += ch.getLocalBounds().size.x + 1.5f;
                }
            };
            drawColoredPwd(pwdBox.getPosition().x + 10.f, pwdBox.getPosition().y + 6.f, 18);

                sf::RectangleShape regenBtn({32.f, 32.f});
                regenBtn.setPosition({pwdBox.getPosition().x + pwdBox.getSize().x - 40.f, pwdBox.getPosition().y + 2.f});
                regenBtn.setFillColor(sf::Color(240, 240, 240));
                regenBtn.setOutlineThickness(1);
                regenBtn.setOutlineColor(sf::Color(200, 200, 200));
                if (regenBtn.getGlobalBounds().contains(mPos))
                {
                    regenBtn.setFillColor(sf::Color(220, 225, 255));
                    if (click)
                    {
                    regeneratePassword();
                }
                }
                window.draw(regenBtn);
                iconRenew.setPosition({regenBtn.getPosition().x + 16.f, regenBtn.getPosition().y + 16.f});
                iconRenew.setColor(sf::Color(50, 50, 60));
                window.draw(iconRenew);

                // Strength
            sf::Color strengthColor = sf::Color(0, 120, 80);
            if (genStrength.label == "Weak") strengthColor = CLR_ERR;
            else if (genStrength.label == "Fair") strengthColor = sf::Color(245, 180, 60);
            else if (genStrength.label == "Strong") strengthColor = sf::Color(0, 150, 120);
            sf::Text stTxt = createUtf8Text("Strength: " + genStrength.label, 16, strengthColor);
            stTxt.setStyle(sf::Text::Bold);
        stTxt.setPosition({panel.getPosition().x + 10.f, panelY + 50.f});
        window.draw(stTxt);

                // Length slider
                int minLen = 4, maxLen = 32;
                float sliderX = panel.getPosition().x + 10.f;
                float sliderY = panelY + 85.f;
                float sliderW = panel.getSize().x - 20.f;
                sf::RectangleShape track({sliderW, 5.f});
                track.setPosition({sliderX, sliderY});
                track.setFillColor(sf::Color(220, 220, 230));
                window.draw(track);

            float knobX = sliderX + ((pendingGenLength - minLen) / static_cast<float>(maxLen - minLen)) * sliderW;
            sf::CircleShape knob(8.f);
            knob.setOrigin({8.f, 8.f});
            knob.setPosition({knobX, sliderY + 2.5f});
            knob.setFillColor(CLR_ACCENT);
            inlineWasDraggingLen = inlineDraggingLen;
            bool overLen = sf::FloatRect({sliderX, sliderY - 8.f}, {sliderW, 20.f}).contains(mPos);
            bool pressedLen = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            if (!inlineDraggingLen && pressedLen && overLen) inlineDraggingLen = true;
            if (inlineDraggingLen && pressedLen)
            {
                float clampedX = std::clamp(mPos.x, sliderX, sliderX + sliderW);
                float t = std::clamp((clampedX - sliderX) / sliderW, 0.f, 1.f);
                int newLen = static_cast<int>(minLen + t * (maxLen - minLen) + 0.5f);
                pendingGenLength = newLen;
            }
            if (!pressedLen) inlineDraggingLen = false;
            if (!inlineDraggingLen && inlineWasDraggingLen && pendingGenLength != genOptions.length)
            {
                genOptions.length = pendingGenLength;
                genSliderDirty = true;
                genSliderClock.restart();
            }
            window.draw(knob);

                sf::Text lenLbl = createUtf8Text(std::to_string(pendingGenLength) + " chars", 14, sf::Color::Black);
                lenLbl.setPosition({sliderX, sliderY + 12.f});
                window.draw(lenLbl);

                // Toggles
                auto smallCheckbox = [&](const std::string& label, float x, float y, bool& ref) -> bool
                {
                    bool changed = false;
                    sf::RectangleShape box({18.f, 18.f});
                    box.setPosition({x, y});
                    box.setFillColor(sf::Color::White);
                    box.setOutlineThickness(1);
                    box.setOutlineColor(sf::Color(180, 180, 180));
                    if (box.getGlobalBounds().contains(mPos))
                    {
                        box.setFillColor(sf::Color(230, 235, 255));
                        if (click)
                        {
                            ref = !ref;
                            changed = true;
                        }
                    }
                    window.draw(box);
                if (ref)
                {
                    iconCheck.setPosition({x + 9.f, y + 9.f});
                    iconCheck.setColor(CLR_ACCENT);
                    window.draw(iconCheck);
                }
                else
                {
                    iconBeforeCheck.setPosition({x + 9.f, y + 9.f});
                    iconBeforeCheck.setColor(sf::Color(160, 160, 160));
                    window.draw(iconBeforeCheck);
                }
                sf::Text lbl = createUtf8Text(label, 14, sf::Color::Black);
                lbl.setPosition({x + 24.f, y - 2.f});
                window.draw(lbl);
                return changed;
            };

                auto applyToggleFix = [&]()
                {
                    if (!genOptions.useLower && !genOptions.useUpper && !genOptions.useDigits && !genOptions.useSymbols)
                        genOptions.useLower = true;
                    regeneratePassword();
                };

                bool toggled = false;
                toggled |= smallCheckbox("a-z", panel.getPosition().x + 10.f, panelY + 120.f, genOptions.useLower);
                toggled |= smallCheckbox("A-Z", panel.getPosition().x + 120.f, panelY + 120.f, genOptions.useUpper);
                toggled |= smallCheckbox("0-9", panel.getPosition().x + 230.f, panelY + 120.f, genOptions.useDigits);
                toggled |= smallCheckbox("!@#$", panel.getPosition().x + 340.f, panelY + 120.f, genOptions.useSymbols);

                // Easy to type toggle
                toggled |= smallCheckbox("Easy to type", panel.getPosition().x + 10.f, panelY + 150.f, genOptions.easyToType);

                if (toggled)
                {
                    applyToggleFix();
                    click = false;
                }

                // Use password button
                sf::RectangleShape useBtn({panel.getSize().x - 20.f, 34.f});
                useBtn.setPosition({panel.getPosition().x + 10.f, panelY + 182.f - 34.f});
                useBtn.setFillColor(CLR_ACCENT);
                if (useBtn.getGlobalBounds().contains(mPos))
                {
                    useBtn.setFillColor(sf::Color(0, 80, 200));
                    if (!inlineDraggingLen && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && click)
                    {
                        buffers["pass"] = toSfStr(generatedPass);
                        showToast("Password applied.");
                    }
                }
                window.draw(useBtn);
                sf::Text useTxt = createUtf8Text("Use password", 16, sf::Color::White);
                useTxt.setPosition({useBtn.getPosition().x + 12.f, useBtn.getPosition().y + 7.f});
                window.draw(useTxt);

                panelHeight = (panel.getSize().y + 10.f);
            }

            // --- Breach check panel ---
            float breachToggleY = toggleY + (showInlinePassGen ? (40.f + panelHeight + 10.f) : 45.f);
            sf::RectangleShape breachTg({600.f, 35.f});
            breachTg.setPosition({SIDE_W + 40, breachToggleY});
            breachTg.setFillColor(sf::Color(245, 247, 250));
            breachTg.setOutlineThickness(1);
            breachTg.setOutlineColor(sf::Color(210, 210, 210));
            bool breachHover = breachTg.getGlobalBounds().contains(mPos);
            if (breachHover) breachTg.setFillColor(sf::Color(235, 240, 250));
            window.draw(breachTg);

            sf::Text bt = createUtf8Text("Check breach status", 15, sf::Color(70, 70, 80));
            bt.setPosition({breachTg.getPosition().x + 12, breachTg.getPosition().y + 8});
            window.draw(bt);

            sf::Sprite caretB = icon_sort;
            caretB.setColor(sf::Color(90, 90, 100));
            caretB.setScale({0.8f, showBreachPanel ? -0.8f : 0.8f});
            sf::FloatRect cb2 = caretB.getLocalBounds();
            caretB.setOrigin({cb2.size.x / 2.f, cb2.size.y / 2.f});
            caretB.setPosition({breachTg.getPosition().x + breachTg.getSize().x - 18.f, breachTg.getPosition().y + 17.f});
            window.draw(caretB);

            if (click && breachHover)
            {
                showBreachPanel = !showBreachPanel;
                if (showBreachPanel && monitor) monitor->update();
                if (!showBreachPanel)
                {
                    hasLiveBreachInfo = false;
                }
            }

            float breachPanelHeight = 0.f;
            if (showBreachPanel)
            {
                float panelY = breachToggleY + 40.f;
                sf::RectangleShape panel({600.f, 100.f});
                panel.setPosition({SIDE_W + 40, panelY});
                panel.setFillColor(sf::Color::White);
                panel.setOutlineThickness(1);
                panel.setOutlineColor(sf::Color(210, 210, 210));
                window.draw(panel);

                std::string label = buffers["eticheta"].toAnsiString();
                std::string currentPwd = u8ToStr(buffers["pass"].toUtf8());

                SecurityMonitor::BreachInfo bi = hasLiveBreachInfo ? liveBreachInfo : SecurityMonitor::BreachInfo{};
                if (!hasLiveBreachInfo && monitor) bi = monitor->getBreachInfo(label);

                sf::Color msgColor = bi.pwned ? CLR_ERR : sf::Color(40, 120, 60);
                if (!bi.onlineAvailable) msgColor = sf::Color(120, 120, 140);
                if (!bi.checked) msgColor = sf::Color(120, 120, 140);

                std::string line = bi.checked ? bi.statusMsg : "Not checked yet.";
            sf::Text st = createUtf8Text(line, 15, msgColor);
                st.setPosition({panel.getPosition().x + 12.f, panelY + 12.f});
                window.draw(st);

                sf::RectangleShape refreshBtn({140.f, 30.f});
                refreshBtn.setPosition({panel.getPosition().x + 12.f, panelY + 55.f});
                refreshBtn.setFillColor(CLR_ACCENT);
                if (refreshBtn.getGlobalBounds().contains(mPos))
                {
                    refreshBtn.setFillColor(sf::Color(0, 80, 200));
                    if (click && monitor)
                    {
                        liveBreachInfo = monitor->runLiveBreachCheck(currentPwd);
                        hasLiveBreachInfo = true;
                        bi = liveBreachInfo;
                    }
                }
                window.draw(refreshBtn);
                sf::Text rt = createUtf8Text("Check now", 15, sf::Color::White);
                rt.setPosition({refreshBtn.getPosition().x + 15.f, refreshBtn.getPosition().y + 7.f});
                window.draw(rt);

                breachPanelHeight = panel.getSize().y + 10.f;
            }

            extraAfterField = (showInlinePassGen ? 40.f + panelHeight : 45.f) +
                (showBreachPanel ? 40.f + breachPanelHeight : 45.f);
        }

        fy += step + extraAfterField;
    }

    // --- BUTOANE JOS (Cancel / Save / Delete) ---
    // (Restul codului pentru butoanele de jos ramane neschimbat, il copiez pentru completitudine)

    // Cancel
    sf::RectangleShape bBack({170.f, 48.f});
    bBack.setPosition({820, 810});
    bBack.setFillColor(sf::Color::Transparent);
    bBack.setOutlineThickness(1.5f);
    bBack.setOutlineColor(sf::Color(180, 180, 180));
    if (click && bBack.getGlobalBounds().contains(mPos))
    {
        appState = AppState::Vault;
        guiError = "";
        showPass = false;
        showCVV = false;
        showInlinePassGen = false;
        showBreachPanel = false;
        showPassGenModal = false;
        hasLiveBreachInfo = false;
        if (backToSecurity)
        {
            currentCat = Category::Security;
            backToSecurity = false;
        }
    }
    window.draw(bBack);
        sf::Text tBack = createUtf8Text("Cancel", 19, sf::Color(120, 120, 120));
    tBack.setPosition({820 + 80.f - tBack.getGlobalBounds().size.x / 2.f, 810 + 10.f});
    window.draw(tBack);

    // Save
    sf::RectangleShape bS({170.f, 48.f});
    bS.setPosition({1000, 810});
    bS.setFillColor(CLR_ACCENT);
    if (bS.getGlobalBounds().contains(mPos)) bS.setFillColor(sf::Color(0, 80, 200));

    // Delete (in Edit Mode)
    if (appState == AppState::Edit && activeItem != nullptr)
    {
        sf::RectangleShape bDel({45.f, 45.f});
        bDel.setPosition({750.f, 810.f});
        bDel.setFillColor(sf::Color(255, 230, 230));
        bDel.setOutlineThickness(1);
        bDel.setOutlineColor(CLR_ERR);
        if (bDel.getGlobalBounds().contains(mPos)) bDel.setFillColor(CLR_ERR);
        if (click && bDel.getGlobalBounds().contains(mPos) && !showDeleteConfirm)
        {
            itemPendingDelete = activeItem;
            showDeleteConfirm = true;
        }
        window.draw(bDel);
        iconTrash.setPosition({750.f + 22.5f, 810.f + 22.5f});
        iconTrash.setColor(bDel.getGlobalBounds().contains(mPos) ? sf::Color::White : CLR_ERR);
        window.draw(iconTrash);
    }

    if ((click && bS.getGlobalBounds().contains(mPos)) || triggerSave)
    {
        // ... (Logica de salvare ramane identica cu ce aveam inainte) ...
        // ... (Include codul din pasii anteriori pentru salvare) ...
        // Poti pastra blocul try/catch existent aici.
        try
        {
            auto& gp = GestionarParole::getInstance();
            if (!gp.getUserCrt()) throw EroareValidare("System", "User not logged in!");

            std::map<std::string, std::string> date;
            for (auto const& [k, v] : buffers) date[k] = u8ToStr(v.toUtf8());

            if (date.count("url") && !date["url"].empty())
            {
                if (date["url"].find("http") == std::string::npos) date["url"] = "https://" + date["url"];
            }
            // ... Validari conturi ...
            if (currentCat == Category::Accounts)
            {
                std::string p = date["pass"];
                const auto& cfg = Configuratie::getInstance();
                if (p.length() < cfg.getLungimeMinimaParola())
                    throw EroareValidare(date["eticheta"], static_cast<int>(p.length()),
                                         static_cast<int>(cfg.getLungimeMinimaParola()));
                // ... restul validarii parolei ...
                bool hasM = false, hasS = false;
                std::string sym = cfg.getCaractereSpecialeValide();
                for (char c : p)
                {
                    if (isupper(c)) hasM = true;
                    if (sym.find(c) != std::string::npos) hasS = true;
                }
                if (!hasM || !hasS) throw EroareValidare(date["eticheta"], "Password needs Uppercase + Symbol.");
            }

            // ADD Logic
            if (appState == AppState::Add)
            {
                if (currentCat == Category::Accounts)
                {
                    date["platforma"] = date["eticheta"];
                    date["utilizator"] = date["user"];
                    date["parola"] = date["pass"];
                    gp.adaugaObiectInSeif("DateAutentificare", date);
                }
                else if (currentCat == Category::Cards)
                {
                    date["numar"] = date["num"];
                    date["dataExpirare"] = date["exp"];
                    date["numeDetinator"] = date["holder"];
                    gp.adaugaObiectInSeif("CardBancar", date);
                }
                else if (currentCat == Category::Notes)
                {
                    date["notita"] = date["note"];
                    gp.adaugaObiectInSeif("NotitaSecurizata", date);
                }
                else if (currentCat == Category::Identities)
                {
                    date["nume"] = date["ln"];
                    date["prenume"] = date["fn"];
                    date["telefon"] = date["tel"];
                    date["email"] = date["em"];
                    date["strada"] = date["str"];
                    date["oras"] = date["ors"];
                    date["judet"] = date["jud"];
                    date["tara"] = date["tar"];
                    date["codPostal"] = date["cp"];
                    gp.adaugaObiectInSeif("Identitate", date);
                }
            }
            // EDIT Logic
            else if (activeItem)
            {
                activeItem->setEticheta(date["eticheta"]);
                if (auto* da = dynamic_cast<DateAutentificare*>(activeItem))
                {
                    da->setter_parola(date["pass"]);
                    da->setter_numeUtilizator(date["user"]);
                    da->setter_url(date["url"]);
                    da->setter_note(date["note"]);
                }
                else if (auto* cb = dynamic_cast<CardBancar*>(activeItem))
                {
                    cb->setNumar(date["num"]);
                    cb->setDataExp(date["exp"]);
                    cb->setCVV(date["cvv"]);
                    cb->setNumeDetinator(date["holder"]);
                }
                else if (auto* ns = dynamic_cast<NotitaSecurizata*>(activeItem))
                {
                    ns->set_notita(date["note"]);
                }
                else if (auto* id = dynamic_cast<Identitate*>(activeItem))
                {
                    id->set_nume(date["ln"]);
                    id->set_prenume(date["fn"]);
                    id->set_telefon(date["tel"]);
                    id->set_email(date["em"]);
                    id->set_strada(date["str"]);
                    id->set_oras(date["ors"]);
                    id->set_judet(date["jud"]);
                    id->set_tara(date["tar"]);
                    id->set_cod_postal(date["cp"]);
                }
            }
            appState = AppState::Vault;
            gp.salveazaDatelePeDisc();
            guiError = "";
            showPass = false;
            showCVV = false;
            triggerSave = false;
            showInlinePassGen = false;
            showBreachPanel = false;
            showPassGenModal = false;
            hasLiveBreachInfo = false;
            if (backToSecurity)
            {
                currentCat = Category::Security;
                securityNeedsRefresh = true;
                backToSecurity = false;
            }
        }
        catch (const ScriptException& e)
        {
            guiError = std::string(e.what()) + "\n" + e.getSugestie();
        }
    }
    window.draw(bS);
    sf::Text tSave = createUtf8Text("Save Changes", 19, sf::Color::White);
    tSave.setPosition({1000 + 80.f - tSave.getGlobalBounds().size.x / 2.f, 810 + 10.f});
    window.draw(tSave);

    if (!guiError.empty())
    {
        sf::Text err = createUtf8Text(guiError, 16, CLR_ERR);
        err.setPosition({SIDE_W + 40, 750});
        window.draw(err);
    }
}

// =========================================================
//  HELPER METHODS
// =========================================================

/**
 * @brief Starts a toast message and resets its timer.
 * @param msg Message text to display.
 */
void UserInterface::showToast(const std::string& msg)
{
    toastMsg = msg;
    toastClock.restart();
}

/**
 * @brief Draws the toast notification if it is still active.
 */
void UserInterface::drawToast()
{
    if (toastMsg.empty() || toastClock.getElapsedTime().asSeconds() > 2.0f) return;

    // Un dreptunghi negru semitransparent in partea de jos-centru
    sf::RectangleShape bg({300.f, 40.f});
    bg.setOrigin({150.f, 20.f}); // Centru
    bg.setPosition({600.f, 850.f}); // Josul ecranului
    bg.setFillColor(sf::Color(40, 40, 40, 220));
    bg.setOutlineThickness(1);
    bg.setOutlineColor(sf::Color(100, 100, 100));

    sf::Text txt = createUtf8Text(toastMsg, 16, sf::Color::White);
    sf::FloatRect bounds = txt.getLocalBounds();
    txt.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f));
    txt.setPosition({600.f, 850.f});

    window.draw(bg);
    window.draw(txt);
}

/**
 * @brief Draws the delete confirmation modal and handles its clicks.
 */
void UserInterface::drawConfirmationModal()
{
    if (!showDeleteConfirm) return;

    // 1. Overlay (Fundal intunecat care blocheaza click-urile din spate)
    sf::RectangleShape overlay({1200.f, 900.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // 2. Fereastra Dialog
    sf::RectangleShape box({400.f, 200.f});
    box.setOrigin({200.f, 100.f});
    box.setPosition({600.f, 450.f});
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setOutlineColor(CLR_ACCENT);
    window.draw(box);

    // 3. Text Intrebare
    sf::Text title = createUtf8Text("Are you sure?", 22, sf::Color::Black);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f));
    title.setPosition({600.f, 400.f});
    window.draw(title);

    sf::Text sub = createUtf8Text("This action cannot be undone.", 16, sf::Color(100, 100, 100));
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setOrigin(sf::Vector2f(sb.position.x + sb.size.x / 2.f, sb.position.y + sb.size.y / 2.f));
    sub.setPosition({600.f, 430.f});
    window.draw(sub);

    // 4. Butoane (YES / NO)

    // -- DELETE BUTTON (Rosu) --
    sf::RectangleShape btnYes({120.f, 40.f});
    btnYes.setPosition({600.f - 130.f, 500.f});
    btnYes.setFillColor(CLR_ERR);
    if (btnYes.getGlobalBounds().contains(mPos)) btnYes.setFillColor(sf::Color(200, 40, 50));
    window.draw(btnYes);

    sf::Text tYes = createUtf8Text("Delete", 16, sf::Color::White);
    tYes.setPosition({btnYes.getPosition().x + 35, btnYes.getPosition().y + 10});
    window.draw(tYes);

    // -- CANCEL BUTTON (Gri) --
    sf::RectangleShape btnNo({120.f, 40.f});
    btnNo.setPosition({600.f + 10.f, 500.f});
    btnNo.setFillColor(sf::Color(200, 200, 200));
    if (btnNo.getGlobalBounds().contains(mPos)) btnNo.setFillColor(sf::Color(180, 180, 180));
    window.draw(btnNo);

    sf::Text tNo = createUtf8Text("Cancel", 16, sf::Color::Black);
    tNo.setPosition({btnNo.getPosition().x + 35, btnNo.getPosition().y + 10});
    window.draw(tNo);

    // 5. Logica Click
    if (click)
    {
        if (btnYes.getGlobalBounds().contains(mPos))
        {
            // Executa Stergerea
            if (itemPendingDelete && GestionarParole::getInstance().getUserCrt())
            {
                GestionarParole::getInstance().getUserCrt()->stergeObiect(itemPendingDelete->getEticheta());
                showToast("Item deleted successfully.");

                // Resetam stari
                appState = AppState::Vault; // Daca eram in Edit, ne intoarce in Vault
                activeItem = nullptr;
                itemPendingDelete = nullptr;
                buffers.clear();
            }
            showDeleteConfirm = false;
        }
        else if (btnNo.getGlobalBounds().contains(mPos))
        {
            // Anuleaza
            showDeleteConfirm = false;
            itemPendingDelete = nullptr;
        }
    }
}

/**
 * @brief Creates a styled SFML text object using the application font.
 */
sf::Text UserInterface::createUtf8Text(const std::string& str, unsigned int size, sf::Color color) const
{
    // Uses the 'font' reference initialized in the constructor
    sf::Text text(font, sf::String::fromUtf8(str.begin(), str.end()), size);
    text.setFillColor(color);
    return text;
}

/**
 * @brief Converts SFML UTF-8 string to standard string.
 */
std::string UserInterface::u8ToStr(const sf::U8String& u8str) const
{
    return std::string(reinterpret_cast<const char*>(u8str.data()), u8str.size());
}

/**
 * @brief Converts standard string to SFML UTF-8 string.
 */
sf::String UserInterface::toSfStr(const std::string& s) const
{
    return sf::String::fromUtf8(s.begin(), s.end());
}

/**
 * @brief Calculates cursor index based on mouse X position within text.
 */
size_t UserInterface::getIndexAtMouse(const sf::Text& text, float mouseX) const
{
    float localX = mouseX - text.getPosition().x;
    if (localX <= 0) return 0;

    const sf::String& s = text.getString();
    for (size_t i = 0; i < s.getSize(); ++i)
    {
        if (localX < text.findCharacterPos(i + 1).x - text.getPosition().x)
            return i;
    }
    return s.getSize();
}

void UserInterface::regeneratePassword()
{
    generatedPass = PasswordGenerator::generate(genOptions);
    genStrength = PasswordGenerator::evaluateStrength(generatedPass);

    static std::array<sf::Color, 6> palette = {
        sf::Color(0, 106, 255),
        sf::Color(220, 53, 69),
        sf::Color(255, 193, 7),
        sf::Color(40, 167, 69),
        sf::Color(111, 66, 193),
        sf::Color(23, 162, 184)
    };
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(palette.size() - 1));

    generatedColors.clear();
    generatedColors.reserve(generatedPass.size());
    for (size_t i = 0; i < generatedPass.size(); ++i)
        generatedColors.push_back(palette[static_cast<size_t>(dist(rng))]);
    pendingGenLength = static_cast<int>(generatedPass.size());
}

/**
 * @brief Draws the sorting modal used in the vault list.
 */
void UserInterface::drawSortModal()
{
    if (!showSortModal) return;

    // 1. Configurare Optiuni
    struct SortOption
    {
        std::string label;
        SortCriteria crit;
    };
    std::vector<SortOption> options;

    if (currentCat == Category::Cards)
    {
        options.push_back({"Title", SortCriteria::Label});
        options.push_back({"Cardholder Name", SortCriteria::CardHolder});
        options.push_back({"Expiration Date", SortCriteria::ExpiryDate});
    }
    else
    {
        options.push_back({"Title", SortCriteria::Label});
        tempCriteria = SortCriteria::Label;
    }

    // 2. Calcul Dimensiuni (Mult mai mari)
    float winW = 1200.f;
    float winH = 900.f;

    // Latime crescuta la 500px
    float modalW = 500.f;

    // Inaltimi componente (mai relaxate)
    float hHeader = 80.f;
    // Rânduri mai înalte (50px box + 15px spatiu = 65px step)
    float hSortSection = 40.f + (options.size() * 65.f);
    float hOrderSection = 110.f;
    float hFooter = 100.f;

    float contentH = hHeader + hSortSection + hOrderSection + hFooter;

    float startX = (winW - modalW) / 2.f;
    float startY = (winH - contentH) / 2.f;

    // --- DESENARE ---

    // A. Overlay
    sf::RectangleShape overlay({winW, winH});
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);

    // B. Fereastra Alba (Modal)
    sf::RectangleShape modal({modalW, contentH});
    modal.setPosition({startX, startY});
    modal.setFillColor(sf::Color::White);
    modal.setOutlineThickness(1);
    modal.setOutlineColor(sf::Color(200, 200, 200));
    sf::FloatRect modalBounds({startX, startY}, {modalW, contentH});

    sf::RectangleShape shadow({modalW, contentH});
    shadow.setPosition({startX + 8.f, startY + 8.f});
    shadow.setFillColor(sf::Color(0, 0, 0, 40));
    window.draw(shadow);
    window.draw(modal);

    // C. Header
    sf::Text title = createUtf8Text("Sort items", 24, sf::Color::Black); // Font mai mare (24)
    title.setPosition({startX + 40.f, startY + 30.f});
    window.draw(title);

    sf::Text closeX = createUtf8Text("X", 22, sf::Color(150, 150, 150));
    closeX.setPosition({startX + modalW - 50.f, startY + 30.f});

    sf::FloatRect xBounds({startX + modalW - 60.f, startY + 20.f}, {50.f, 50.f});
    if (click && xBounds.contains(mPos))
    {
        showSortModal = false;
        sortModalArmed = false;
        click = false;
    }
    window.draw(closeX);

    // Arm to ignore the opening click; close on outside click after release
    if (!sortModalArmed)
    {
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) sortModalArmed = true;
        click = false;
    }
    else if (click && !modalBounds.contains(mPos))
    {
        showSortModal = false;
        sortModalArmed = false;
        click = false;
        return;
    }

    // Close on outside click
    if (click && !modalBounds.contains(mPos))
    {
        showSortModal = false;
        click = false;
        return;
    }

    float currentY = startY + hHeader;

    // D. Sectiunea "Sort by"
    sf::Text lblBy = createUtf8Text("Sort by", 16, sf::Color::Black);
    lblBy.setStyle(sf::Text::Bold);
    lblBy.setPosition({startX + 40.f, currentY});
    window.draw(lblBy);

    currentY += 35.f; // Spatiu sub label

    for (const auto& opt : options)
    {
        sf::RectangleShape box({modalW - 80.f, 50.f}); // Mai inalte (50px)
        box.setPosition({startX + 40.f, currentY});

        bool isSelected = (tempCriteria == opt.crit);
        box.setFillColor(isSelected ? sf::Color(240, 248, 255) : sf::Color(250, 250, 250));
        box.setOutlineThickness(isSelected ? 1.5f : 0.f);
        box.setOutlineColor(CLR_ACCENT);

        if (click && box.getGlobalBounds().contains(mPos)) tempCriteria = opt.crit;
        window.draw(box);

        sf::Text txt = createUtf8Text(opt.label, 17, sf::Color::Black); // Text mai mare
        // Centrare verticala (50px inaltime box)
        txt.setPosition({startX + 55.f, currentY + 13.f});
        window.draw(txt);

        if (isSelected)
        {
            sf::Text check = createUtf8Text("V", 16, CLR_ACCENT);
            check.setPosition({startX + modalW - 80.f, currentY + 13.f});
            window.draw(check);
        }
        currentY += 65.f; // 50px box + 15px spatiu
    }

    // E. Sectiunea "Order"
    currentY += 15.f; // Spatiu extra

    sf::Text lblOrd = createUtf8Text("Order", 16, sf::Color::Black);
    lblOrd.setStyle(sf::Text::Bold);
    lblOrd.setPosition({startX + 40.f, currentY});
    window.draw(lblOrd);

    currentY += 40.f;

    // Butoane Order (mai late si mai inalte)
    float btnW = (modalW - 90.f) / 2.f;

    auto drawOrderBtn = [&](float xOffset, std::string txt, SortOrder ord)
    {
        sf::RectangleShape btn({btnW, 50.f}); // Inaltime 50px
        float btnX = startX + 40.f + xOffset;
        float btnY = currentY;

        btn.setPosition({btnX, btnY});
        bool active = (tempSortOrder == ord);

        btn.setFillColor(active ? sf::Color(240, 248, 255) : sf::Color(250, 250, 250));
        btn.setOutlineThickness(active ? 1.5f : 1.f);
        btn.setOutlineColor(active ? CLR_ACCENT : sf::Color(220, 220, 220));

        if (click && btn.getGlobalBounds().contains(mPos)) tempSortOrder = ord;
        window.draw(btn);

        sf::Text t = createUtf8Text(txt, 16, active ? CLR_ACCENT : sf::Color::Black);
        sf::FloatRect tb = t.getLocalBounds();
        t.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
        t.setPosition({btnX + btnW / 2.f, btnY + 25.f}); // Centrat pe inaltimea de 50px
        window.draw(t);
    };

    drawOrderBtn(0.f, "Descending", SortOrder::Descending);
    drawOrderBtn(btnW + 10.f, "Ascending", SortOrder::Ascending);

    // F. Butonul APPLY
    currentY += 90.f; // Spatiu generos jos

    sf::RectangleShape btnApply({modalW - 80.f, 55.f}); // Buton masiv (55px inaltime)
    btnApply.setPosition({startX + 40.f, currentY});
    btnApply.setFillColor(CLR_ACCENT);

    if (btnApply.getGlobalBounds().contains(mPos)) btnApply.setFillColor(sf::Color(0, 80, 200));
    window.draw(btnApply);

    sf::Text tApply = createUtf8Text("Apply Changes", 18, sf::Color::White); // Font 18
    sf::FloatRect tb = tApply.getLocalBounds();
    tApply.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
    tApply.setPosition({startX + modalW / 2.f, currentY + 27.5f});
    window.draw(tApply);

    // Logic Apply
    if (click && btnApply.getGlobalBounds().contains(mPos))
    {
        activeSortOrder = tempSortOrder;
        activeCriteria = tempCriteria;
        showSortModal = false;
        click = false;
    }
}

/**
 * @brief Renders the password generator modal (standalone or linked to the password field).
 */
void UserInterface::drawPasswordGeneratorModal()
{
    if (!showPassGenModal) return;

    auto regenerate = [&]()
    {
        regeneratePassword();
    };

    sf::Vector2u winSize = window.getSize();
    float winW = static_cast<float>(winSize.x);
    float winH = static_cast<float>(winSize.y);

    sf::RectangleShape overlay({winW, winH});
    overlay.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(overlay);

    float modalW = 560.f, modalH = 430.f;
    float startX = (winW - modalW) / 2.f;
    float startY = (winH - modalH) / 2.f;
    sf::FloatRect modalBounds({startX, startY}, {modalW, modalH});

    // Arm modal after the opening click; swallow that click
    if (!passGenModalArmed)
    {
        passGenModalArmed = true;
        click = false;
    }

    // Close when clicking outside the modal (but keep UI visible underneath)
    if (passGenModalArmed && click && !modalBounds.contains(mPos))
    {
        showPassGenModal = false;
        passGenModalArmed = false;
        modalDraggingLen = false;
        modalWasDraggingLen = false;
        genOptions.length = DEFAULT_GEN_LEN;
        pendingGenLength = DEFAULT_GEN_LEN;
        genSliderDirty = false;
        regeneratePassword();
        click = false;
        return;
    }

    sf::RectangleShape modal({modalW, modalH});
    modal.setPosition({startX, startY});
    modal.setFillColor(sf::Color::White);
    modal.setOutlineThickness(1.5f);
    modal.setOutlineColor(sf::Color(210, 210, 210));
    window.draw(modal);

    sf::Text title = createUtf8Text("Generate password", 22, sf::Color::Black);
    title.setPosition({startX + 20.f, startY + 15.f});
    window.draw(title);

    // Close button
    sf::Text closeX = createUtf8Text("X", 20, sf::Color(150, 150, 150));
    closeX.setPosition({startX + modalW - 35.f, startY + 15.f});
    sf::FloatRect xBounds({startX + modalW - 45.f, startY + 5.f}, {40.f, 40.f});
    if (click && xBounds.contains(mPos))
    {
        showPassGenModal = false;
        passGenModalArmed = false;
        modalDraggingLen = false;
        modalWasDraggingLen = false;
        genOptions.length = DEFAULT_GEN_LEN;
        pendingGenLength = DEFAULT_GEN_LEN;
        genSliderDirty = false;
        regeneratePassword();
        click = false;
        return;
    }
    window.draw(closeX);

    // Generated password box
    sf::RectangleShape pwdBox({modalW - 40.f, 60.f});
    pwdBox.setPosition({startX + 20.f, startY + 60.f});
    pwdBox.setFillColor(sf::Color(248, 248, 250));
    pwdBox.setOutlineThickness(1);
    pwdBox.setOutlineColor(sf::Color(220, 220, 220));
    window.draw(pwdBox);

    auto drawColoredPwdModal = [&](float x, float y, unsigned int sz)
    {
        float cx = x;
        for (size_t i = 0; i < generatedPass.size(); ++i)
        {
            sf::Text ch = createUtf8Text(std::string(1, generatedPass[i]), sz,
                                         i < generatedColors.size() ? generatedColors[i] : sf::Color::Black);
            ch.setPosition({cx, y});
            window.draw(ch);
            cx += ch.getLocalBounds().size.x + 2.f;
        }
    };
    drawColoredPwdModal(startX + 30.f, startY + 78.f, 22);

    // Regenerate button (icon)
    sf::RectangleShape regenBtn({32.f, 32.f});
    regenBtn.setPosition({startX + modalW - 60.f, startY + 74.f});
    regenBtn.setFillColor(sf::Color(240, 240, 240));
    regenBtn.setOutlineThickness(1);
    regenBtn.setOutlineColor(sf::Color(200, 200, 200));
    if (regenBtn.getGlobalBounds().contains(mPos))
    {
        regenBtn.setFillColor(sf::Color(220, 225, 255));
        if (click) regenerate();
    }
    window.draw(regenBtn);
    iconRenew.setPosition({regenBtn.getPosition().x + 16, regenBtn.getPosition().y + 16});
    iconRenew.setColor(sf::Color(50, 50, 60));
    window.draw(iconRenew);

    // Use / Copy button
    sf::RectangleShape useBtn({modalW - 40.f, 45.f});
    useBtn.setPosition({startX + 20.f, startY + 130.f});
    useBtn.setFillColor(CLR_ACCENT);
    if (useBtn.getGlobalBounds().contains(mPos))
    {
        useBtn.setFillColor(sf::Color(0, 80, 200));
        if (click)
        {
            if (!generatorTargetKey.empty())
            {
                buffers[generatorTargetKey] = toSfStr(generatedPass);
                showToast("Password applied.");
            }
            else
            {
                sf::Clipboard::setString(generatedPass);
                showToast("Password copied.");
            }
            showPassGenModal = false;
            passGenModalArmed = false;
            modalDraggingLen = false;
            modalWasDraggingLen = false;
            genOptions.length = DEFAULT_GEN_LEN;
            pendingGenLength = DEFAULT_GEN_LEN;
            genSliderDirty = false;
            regeneratePassword();
            click = false;
            return;
        }
    }
    window.draw(useBtn);
    sf::Text useTxt = createUtf8Text("Use password", 18, sf::Color::White);
    useTxt.setPosition({useBtn.getPosition().x + 15, useBtn.getPosition().y + 12});
    window.draw(useTxt);

    // Strength label
    sf::Color strengthColor = sf::Color(0, 120, 80);
    if (genStrength.label == "Weak") strengthColor = CLR_ERR;
    else if (genStrength.label == "Fair") strengthColor = sf::Color(245, 180, 60);
    else if (genStrength.label == "Strong") strengthColor = sf::Color(0, 150, 120);
    sf::Text stTxt = createUtf8Text("Password strength: " + genStrength.label, 17, strengthColor);
    stTxt.setStyle(sf::Text::Bold);
    stTxt.setPosition({startX + 20.f, startY + 190.f});
    window.draw(stTxt);

    // Length slider
    int minLen = 4;
    int maxLen = 32;
    float sliderY = startY + 240.f;
    float sliderX = startX + 25.f;
    float sliderW = modalW - 50.f;

    sf::RectangleShape track({sliderW, 6.f});
    track.setPosition({sliderX, sliderY});
    track.setFillColor(sf::Color(220, 220, 230));
    window.draw(track);

    float knobX = sliderX + ((pendingGenLength - minLen) / static_cast<float>(maxLen - minLen)) * sliderW;
    sf::CircleShape knob(10.f);
    knob.setOrigin({10.f, 10.f});
    knob.setPosition({knobX, sliderY + 3.f});
    knob.setFillColor(CLR_ACCENT);
    modalWasDraggingLen = modalDraggingLen;
    bool over = sf::FloatRect({sliderX, sliderY - 10.f}, {sliderW, 26.f}).contains(mPos);
    bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    if (!modalDraggingLen && pressed && over) modalDraggingLen = true;
    if (modalDraggingLen && pressed)
    {
        float clampedX = std::clamp(mPos.x, sliderX, sliderX + sliderW);
        float t = std::clamp((clampedX - sliderX) / sliderW, 0.f, 1.f);
        int newLen = static_cast<int>(minLen + t * (maxLen - minLen) + 0.5f);
        pendingGenLength = newLen;
    }
    if (!pressed) modalDraggingLen = false;
    if (!modalDraggingLen && modalWasDraggingLen && pendingGenLength != genOptions.length)
    {
        genOptions.length = pendingGenLength;
        genSliderDirty = true;
        genSliderClock.restart();
    }
    window.draw(knob);

    sf::Text lenMin = createUtf8Text(std::to_string(minLen), 14, sf::Color::Black);
    lenMin.setPosition({sliderX, sliderY + 18.f});
    window.draw(lenMin);

    sf::Text lenCur = createUtf8Text(std::to_string(pendingGenLength) + " chars", 14, sf::Color::Black);
    lenCur.setPosition({sliderX + sliderW / 2.f - 24.f, sliderY + 18.f});
    window.draw(lenCur);

    sf::Text lenMax = createUtf8Text(std::to_string(maxLen), 14, sf::Color::Black);
    lenMax.setPosition({sliderX + sliderW - 20.f, sliderY + 18.f});
    window.draw(lenMax);

    // Toggle helpers
    auto drawCheckbox = [&](const std::string& label, float x, float y, bool& ref) -> bool
    {
        bool changed = false;
        sf::RectangleShape box({22.f, 22.f});
        box.setPosition({x, y});
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(1);
        box.setOutlineColor(sf::Color(180, 180, 180));
        if (box.getGlobalBounds().contains(mPos))
        {
            box.setFillColor(sf::Color(230, 235, 255));
            if (click)
            {
                ref = !ref;
                changed = true;
            }
        }
        window.draw(box);
        if (ref)
        {
            iconCheck.setPosition({x + 11.f, y + 11.f});
            iconCheck.setColor(CLR_ACCENT);
            window.draw(iconCheck);
        }
        sf::Text lbl = createUtf8Text(label, 15, sf::Color::Black);
        lbl.setPosition({x + 30.f, y - 2.f});
        window.draw(lbl);
        return changed;
    };

    // All characters vs Easy to type (radio style)
    auto drawRadio = [&](const std::string& label, float x, float y, bool value, auto onClick) -> bool
    {
        bool changed = false;
        sf::CircleShape circ(11.f);
        circ.setPosition({x, y});
        circ.setFillColor(sf::Color::White);
        circ.setOutlineThickness(1);
        circ.setOutlineColor(sf::Color(180, 180, 180));
        if (circ.getGlobalBounds().contains(mPos))
        {
            circ.setFillColor(sf::Color(230, 235, 255));
            if (click)
            {
                onClick();
                changed = true;
            }
        }
        window.draw(circ);
        if (value)
        {
            sf::CircleShape dot(6.f);
            dot.setPosition({x + 5.f, y + 5.f});
            dot.setFillColor(CLR_ACCENT);
            window.draw(dot);
        }
        sf::Text lbl = createUtf8Text(label, 15, sf::Color::Black);
        lbl.setPosition({x + 26.f, y - 2.f});
        window.draw(lbl);
        return changed;
    };

    float toggleY = startY + 290.f;
    bool changed = false;
    changed |= drawRadio("All characters", startX + 20.f, toggleY, !genOptions.easyToType, [&]()
    {
        genOptions.easyToType = false;
    });
    changed |= drawRadio("Easy to type", startX + 210.f, toggleY, genOptions.easyToType, [&]()
    {
        genOptions.easyToType = true;
    });

    float row1Y = toggleY + 40.f;
    changed |= drawCheckbox("a-z", startX + 20.f, row1Y, genOptions.useLower);
    changed |= drawCheckbox("A-Z", startX + 210.f, row1Y, genOptions.useUpper);

    float row2Y = row1Y + 40.f;
    changed |= drawCheckbox("0-9", startX + 20.f, row2Y, genOptions.useDigits);
    changed |= drawCheckbox("!@#", startX + 210.f, row2Y, genOptions.useSymbols);

    if (changed)
    {
        if (!genOptions.useLower && !genOptions.useUpper && !genOptions.useDigits && !genOptions.useSymbols)
            genOptions.useLower = true;
        regenerate();
        click = false;
    }
}
