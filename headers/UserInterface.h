/**
 * @file UserInterface.h
 * @brief Declares the main GUI wrapper and UI state.
 */
#ifndef OOP_USERINTERFACE_H
#define OOP_USERINTERFACE_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <optional>
#include "GestionarParole.h"
#include "SecurityMonitor.h"
#include "PasswordGenerator.h"

// Constants for UI styling
constexpr sf::Color CLR_NAVY(5, 15, 35);
constexpr sf::Color CLR_ACCENT(0, 106, 255);
const sf::Color CLR_BG(245, 246, 248);
constexpr sf::Color CLR_ERR(220, 53, 69);
const sf::Color CLR_SELECT(0, 106, 255, 100);
const float SIDE_W = 240.f;

/**
 * @brief High-level UI state machine.
 */
enum class AppState { Vault, Edit, Add };

/**
 * @brief Active vault category shown in the main list.
 */
enum class Category { Accounts, Identities, Cards, Notes, Security };

/**
 * @class UserInterface
 * @brief Main application wrapper handling the GUI, Event Loop, and State.
 */
class UserInterface
{
private:
    // ... (variabile grafice existente: window, font, logo, iconite meniu etc.) ...
    sf::RenderWindow window;
    sf::Font& font;
    sf::Sprite logoSprite;
    sf::Text logoText;

    // Iconite
    sf::Sprite iconMenu, iconCopy, iconGlobe, iconEdit, iconTrash;
    sf::Sprite iconEyeOpen, iconEyeClosed;
    sf::Sprite iconCheck, iconRenew, iconBeforeCheck;

    // --- [NOU] SEARCH & SORT UI Elements ---
    /// Search icon sprite used in the search bar.
    sf::Sprite icon_search;
    /// Sort icon sprite used in the sort button.
    sf::Sprite icon_sort;
    /// Sort button background.
    sf::RectangleShape sortBtn; // Butonul de sortare (patratul)

    // --- State Variables ---
    AppState appState;
    Category currentCat;
    Seif* activeItem;
    Seif* menuOpenItem;
    sf::Vector2f menuPos;

    // Input buffers
    std::map<std::string, sf::String> buffers;
    std::map<std::string, size_t> cursors;
    std::map<std::string, size_t> anchors;
    std::string focusKey;
    std::string guiError;

    // Interaction flags
    float scrollOffset;
    bool showPass, showCVV, showCursor, triggerSave;
    sf::Clock blinkClock, doubleClickClock;
    bool click, dblClick;
    sf::Vector2f mPos;
    sf::Clock genSliderClock;
    bool genSliderDirty;
    int pendingGenLength;

    // Undo/Redo
    std::map<std::string, std::vector<sf::String>> undoStack;
    std::map<std::string, std::vector<sf::String>> redoStack;

    // Toast & Confirm Delete
    std::string toastMsg;
    sf::Clock toastClock;
    bool showDeleteConfirm;
    Seif* itemPendingDelete;

    // --- [NOU] SEARCH BAR STATE ---
    /// Current search query string.
    std::string searchQuery;
    /// True when the search bar has keyboard focus.
    bool isSearchFocused;
    /// Search bar background shape.
    sf::RectangleShape searchBg;
    /// Rendered search text.
    sf::Text searchText;
    /// Placeholder text shown when the search query is empty.
    sf::Text searchPlaceholder;
    /// Cursor index within searchQuery.
    size_t searchCursor;
    /// Selection anchor index within searchQuery.
    size_t searchAnchor;

    // --- [NOU] SORTING SYSTEM STATE ---
    /**
     * @brief Sorting direction for the vault list.
     */
    enum class SortOrder { Ascending, Descending };

    /**
     * @brief Sorting field for the vault list.
     */
    enum class SortCriteria { Label, CardHolder, ExpiryDate };

    /// True when the sorting modal is visible.
    bool showSortModal; // Este fereastra de sortare deschisa?
    bool sortModalArmed;

    // Setarile ACTIVE (aplicate pe lista)
    /// Applied sort order for the list.
    SortOrder activeSortOrder;
    /// Applied sort criteria for the list.
    SortCriteria activeCriteria;

    // Setarile TEMPORARE (in meniul popup, inainte de Apply)
    /// Pending sort order while the modal is open.
    SortOrder tempSortOrder;
    /// Pending sort criteria while the modal is open.
    SortCriteria tempCriteria;

    // Logic Reference
    SecurityMonitor* monitor;
    bool securityNeedsRefresh;

    // --- Password Generator State ---
    bool showPassGenModal;
    bool showInlinePassGen;
    bool passGenModalArmed;
    bool inlineDraggingLen;
    bool inlineWasDraggingLen;
    bool modalDraggingLen;
    bool modalWasDraggingLen;
    bool backToSecurity;
    bool showBreachPanel;
    bool showSecurityGenPanel;
    bool showDupPanel;
    bool showWeakPanel;
    PasswordGenerator::Options genOptions;
    PasswordGenerator::StrengthInfo genStrength;
    std::string generatedPass;
    std::string generatorTargetKey; ///< Field to autofill (e.g., "pass"), empty when opened from menu.
    bool hasLiveBreachInfo;
    SecurityMonitor::BreachInfo liveBreachInfo;
    std::vector<sf::Color> generatedColors;

    // --- Helper Functions ---
    /**
     * @brief Initializes UI textures, sprites, layout, and colors.
     */
    void initVisuals();
    /**
     * @brief Loads persisted data and seeds a default user when needed.
     */
    void initMockData();
    /**
     * @brief Configures a UI icon with size, origin, and tint.
     * @param sprite Icon sprite to configure.
     * @param texName Texture name used for size heuristics.
     */
    void setupIcon(sf::Sprite& sprite, const std::string& texName);
    /**
     * @brief Handles input events and updates UI interaction state.
     */
    void processEvents();
    /**
     * @brief Updates UI state between input and rendering.
     */
    void update();
    /**
     * @brief Renders the current UI frame.
     */
    void render();

    /**
     * @brief Draws the left navigation sidebar.
     */
    void drawSidebar();
    /**
     * @brief Draws the main vault list view.
     */
    void drawVault();
    /**
     * @brief Renders the Security report view.
     */
    void drawSecurityReport();
    /**
     * @brief Draws the edit or add form for the current item.
     */
    void drawEditForm();

    /**
     * @brief Shows a short toast message.
     * @param msg Message text to display.
     */
    void showToast(const std::string& msg);
    /**
     * @brief Renders the toast message if active.
     */
    void drawToast();
    /**
     * @brief Renders the delete confirmation modal.
     */
    void drawConfirmationModal();
    /**
     * @brief Renders the sorting modal for the vault list.
     */
    void drawSortModal(); // [NOU] Deseneaza fereastra de sortare
    /**
     * @brief Renders the password generator modal.
     */
    void drawPasswordGeneratorModal();

    // Utility
    /**
     * @brief Creates an SFML text object using the configured font.
     * @param str UTF-8 string to render.
     * @param size Font size in pixels.
     * @param color Fill color.
     * @return Configured sf::Text instance.
     */
    sf::Text createUtf8Text(const std::string& str, unsigned int size, sf::Color color) const;
    /**
     * @brief Converts an SFML UTF-8 string to std::string.
     * @param u8str Source UTF-8 string.
     * @return Converted std::string.
     */
    std::string u8ToStr(const sf::U8String& u8str) const;
    /**
     * @brief Converts std::string to an SFML UTF-8 string.
     * @param s Source string.
     * @return Converted sf::String.
     */
    sf::String toSfStr(const std::string& s) const;
    /**
     * @brief Computes a cursor index from a mouse X position.
     * @param text Rendered text element.
     * @param mouseX Mouse X coordinate in world space.
     * @return Closest character index within the text.
     */
    size_t getIndexAtMouse(const sf::Text& text, float mouseX) const;

public:
    /**
     * @brief Constructs the UI wrapper and initializes resources.
     */
    UserInterface();
    /**
     * @brief Saves state and shuts down the application cleanly.
     */
    ~UserInterface();
    /**
     * @brief Runs the main event/render loop.
     */
    void run();
    /**
     * @brief Regenerates password, updates strength and per-character colors.
     */
    void regeneratePassword();
};
#endif //OOP_USERINTERFACE_H
