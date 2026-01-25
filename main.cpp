#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <optional>
#include <string>
#include <map>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

#include "GestionarParole.h"
#include "ResourceManager.hpp"
#include "Configuratie.h"
#include "DateAutentificare.h"
#include "CardBancar.h"
#include "NotitaSecurizata.h"
#include "Identitate.h"
#include "SecurityMonitor.h"

// --- Funcție Șablon ---
template <typename T, typename Colectie>
T* gasesteDupaNume(Colectie& colectie, const std::string& cautat)
{
    for (auto& element : colectie)
    {
        if constexpr (std::is_same_v<typename Colectie::value_type, std::shared_ptr<T>>)
        {
            if (element->getEticheta() == cautat) return element.get();
        }
        else
        {
            if (element.getEticheta() == cautat) return &element;
        }
    }
    return nullptr;
}

// --- CONSTANTE DE DESIGN ---
constexpr sf::Color CLR_NAVY(5, 15, 35);
constexpr sf::Color CLR_ACCENT(0, 106, 255);
constexpr sf::Color CLR_BG(245, 246, 248);
constexpr sf::Color CLR_ERR(220, 53, 69);
constexpr sf::Color CLR_SELECT(0, 106, 255, 100);
constexpr float SIDE_W = 240.f;

enum class AppState { Vault, Edit, Add };

enum class Category { Accounts, Identities, Cards, Notes, Security };

// Utilitare conversie String <-> UTF8
std::string u8ToStr(const sf::U8String& u8str)
{
    return std::string(reinterpret_cast<const char*>(u8str.data()), u8str.size());
}

sf::String toSfStr(const std::string& s) { return sf::String::fromUtf8(s.begin(), s.end()); }

// Helper creare text
sf::Text createUtf8Text(const std::string& str, const sf::Font& font, unsigned int size, sf::Color color)
{
    sf::Text text(font, sf::String::fromUtf8(str.begin(), str.end()), size);
    text.setFillColor(color);
    return text;
}

// Helper mouse click
size_t getIndexAtMouse(const sf::Text& text, float mouseX)
{
    float localX = mouseX - text.getPosition().x;
    if (localX <= 0) return 0;
    const sf::String& s = text.getString();
    for (size_t i = 0; i < s.getSize(); ++i)
    {
        if (localX < text.findCharacterPos(i + 1).x - text.getPosition().x) return i;
    }
    return s.getSize();
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    try
    {
        auto& app = GestionarParole::getInstance();
        auto& res = ResourceManager::Instance();

        sf::RenderWindow window(sf::VideoMode({1200, 900}), "S.C.R.I.P.T. Password Manager");
        window.setFramerateLimit(60);
        sf::Font& font = res.getFont("FiraSans-Regular.ttf");

        // --- SETUP LOGO START ---
        sf::Texture logoTex;
        if (!logoTex.loadFromFile("images/logo.png"))
        {
            std::cout << "[WARN] NU s-a gasit imaginea 'images/logo.png'. Copiaz-o langa exe!\n";
        }
        logoTex.setSmooth(true);

        sf::Sprite logoSprite(logoTex);

        // MĂRIMEA DORITĂ (90px)
        float targetHeight = 120.f;
        float texH = static_cast<float>(logoTex.getSize().y);
        float scaleFactor = targetHeight / (texH > 0 ? texH : 1.f);

        // SFML 3.0: Folosim acolade pentru Vector2f
        logoSprite.setScale({scaleFactor, scaleFactor});
        logoSprite.setPosition({-5.f, 25.f});

        // Textul de langa logo
        sf::Text logoText = createUtf8Text("S.C.R.I.P.T", font, 18, sf::Color::White);

        // Calcul pozitie text
        auto logoBounds = logoSprite.getGlobalBounds();
        // SFML 3.0 foloseste .size.x / .size.y
        logoText.setPosition({
            logoBounds.position.x + logoBounds.size.x,
            logoBounds.position.y + (logoBounds.size.y / 2.f) - 15.f
        });
        // --- SETUP LOGO END ---


        // --- INITIALIZARE PERSISTENTA ---
        try
        {
            app.getManager().adauga(Utilizator("Andrei", "andrei@proiect.ro", "secret123"));
            app.login("Andrei", "secret123");

            if (Utilizator* u = app.getUserCrt())
            {
                static SecurityMonitor monitor(*u);
                u->attach(&monitor);
                app.incarcaDateleDePeDisc();

                if (u->getNrObiecte() == 0)
                {
                    u->adaugaObiect(std::make_shared<DateAutentificare>("Facebook", "andrei.nituică",
                                                                        "ParolăValidă123!", "https://www.facebook.com/",
                                                                        "Cont principal"));
                    u->adaugaObiect(std::make_shared<CardBancar>("Revolut", "4556 1234 5678 0000", "12/28", "123"));
                    u->adaugaObiect(std::make_shared<NotitaSecurizata>("Cod Poartă", "Acces: 9988#"));
                    u->adaugaObiect(std::make_shared<Identitate>("Acasă", "Nițuică", "Andrei", "0720x", "a@m.ro",
                                                                 "Str. X", "Otopeni", "IF", "RO", "0771"));
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Eroare initializare: " << e.what() << std::endl;
        }

        AppState appState = AppState::Vault;
        Category currentCat = Category::Accounts;
        Seif* activeItem = nullptr;
        float scrollOffset = 0.f;

        std::map<std::string, sf::String> buffers;
        std::map<std::string, size_t> cursors, anchors;
        std::string focusKey = "", guiError = "";
        bool showPass = false, showCursor = true;
        sf::Clock blinkClock, doubleClickClock;

        while (window.isOpen())
        {
            bool triggerSave = false;
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            bool click = false, dblClick = false;

            if (blinkClock.getElapsedTime().asSeconds() > 0.5f)
            {
                showCursor = !showCursor;
                blinkClock.restart();
            }

            while (const std::optional<sf::Event> event = window.pollEvent())
            {
                if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>())
                {
                    if (scroll->wheel == sf::Mouse::Wheel::Vertical && appState == AppState::Vault)
                        scrollOffset -= scroll->delta * 25.f;
                }

                if (event->is<sf::Event::Closed>()) window.close();

                if (appState != AppState::Vault)
                {
                    if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                    {
                        bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                        bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);

                        if (key->code == sf::Keyboard::Key::A && ctrl)
                        {
                            if (!focusKey.empty())
                            {
                                anchors[focusKey] = 0;
                                cursors[focusKey] = buffers[focusKey].getSize();
                            }
                        }

                        if (key->code == sf::Keyboard::Key::Tab)
                        {
                            std::vector<std::string> order = {"eticheta"};
                            if (currentCat == Category::Accounts)
                                order.insert(
                                    order.end(), {"user", "pass", "url", "note"});
                            else if (currentCat == Category::Cards) order.insert(order.end(), {"num", "exp", "cvv"});
                            else if (currentCat == Category::Identities)
                                order.insert(order.end(), {"fn", "ln", "tel", "em", "str", "ors", "jud", "tar", "cp"});
                            else if (currentCat == Category::Notes) order.push_back("note");

                            auto it = std::ranges::find(order, focusKey);
                            focusKey = (it != order.end())
                                           ? order[(std::distance(order.begin(), it) + 1) % order.size()]
                                           : order[0];
                            cursors[focusKey] = anchors[focusKey] = buffers[focusKey].getSize();
                        }

                        if (key->code == sf::Keyboard::Key::Enter && appState != AppState::Vault)
                        {
                            triggerSave = true;
                        }

                        if (!focusKey.empty())
                        {
                            auto& cur = cursors[focusKey];
                            auto& anc = anchors[focusKey];
                            auto& buf = buffers[focusKey];

                            if (key->code == sf::Keyboard::Key::Left)
                            {
                                if (ctrl) cur = 0;
                                else if (cur > 0) cur--;
                                if (!shift) anc = cur;
                            }
                            if (key->code == sf::Keyboard::Key::Right)
                            {
                                if (ctrl) cur = buf.getSize();
                                else if (cur < buf.getSize()) cur++;
                                if (!shift) anc = cur;
                            }
                            if (key->code == sf::Keyboard::Key::Delete)
                            {
                                if (anc != cur)
                                {
                                    size_t s = std::min(anc, cur), e = std::max(anc, cur);
                                    buf.erase(s, e - s);
                                    cur = anc = s;
                                }
                                else if (cur < buf.getSize()) buf.erase(cur, 1);
                            }
                        }
                    }

                    if (const auto* text = event->getIf<sf::Event::TextEntered>())
                    {
                        bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                        if (!focusKey.empty() && text->unicode != '\t' && text->unicode != '\r' && !ctrl)
                        {
                            auto& cur = cursors[focusKey];
                            auto& anc = anchors[focusKey];
                            auto& buf = buffers[focusKey];
                            if (text->unicode == '\b')
                            {
                                if (anc != cur)
                                {
                                    size_t s = std::min(anc, cur), e = std::max(anc, cur);
                                    buf.erase(s, e - s);
                                    cur = anc = s;
                                }
                                else if (cur > 0)
                                {
                                    buf.erase(cur - 1, 1);
                                    cur--;
                                    anc = cur;
                                }
                            }
                            else if (text->unicode >= 32)
                            {
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

                if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>())
                {
                    if (m->button == sf::Mouse::Button::Left)
                    {
                        click = true;
                        if (doubleClickClock.getElapsedTime().asSeconds() < 0.3f) dblClick = true;
                        doubleClickClock.restart();
                    }
                }
            }

            window.clear(CLR_BG);

            // 1. DESENAM FUNDALUL SIDEBAR (Primul strat)
            sf::RectangleShape sb({SIDE_W, 900.f});
            sb.setFillColor(CLR_NAVY);
            window.draw(sb);

            // 2. DESENAM LOGO-UL (Al doilea strat - peste fundal)
            window.draw(logoSprite);
            window.draw(logoText);

            // 3. DESENAM MENIURILE (Al treilea strat)
            std::vector<std::pair<std::string, Category>> nav = {
                {"Accounts", Category::Accounts},
                {"Identities", Category::Identities},
                {"Credit cards", Category::Cards},
                {"Notes", Category::Notes},
                {"Security report", Category::Security}
            };

            float ny = 140.f; // Coboram meniul putin ca sa nu se suprapuna cu logo-ul mare
            for (auto& n : nav)
            {
                sf::RectangleShape b({SIDE_W, 50.f});
                b.setPosition({0, ny});
                bool isActive = (currentCat == n.second);
                b.setFillColor(isActive ? sf::Color(20, 40, 70) : sf::Color::Transparent);

                if (click && b.getGlobalBounds().contains(mPos))
                {
                    currentCat = n.second;
                    appState = AppState::Vault;
                    guiError = "";
                    cursors.clear();
                    anchors.clear();
                    showPass = false;
                    scrollOffset = 0.f;

                    if (currentCat == Category::Security && app.getUserCrt())
                    {
                        app.getUserCrt()->notify();
                    }
                }
                window.draw(b);

                sf::Text t = createUtf8Text(n.first, font, 16, isActive ? sf::Color::White : sf::Color(150, 160, 180));
                t.setPosition({50, ny + 12});
                window.draw(t);
                ny += 55.f;
            }

            if (appState == AppState::Vault)
            {
                std::string titleStr = "My Vault";
                if (currentCat == Category::Security) titleStr = "Security Analysis";

                sf::Text title = createUtf8Text(titleStr, font, 26, sf::Color::Black);
                title.setPosition({SIDE_W + 30, 30});
                window.draw(title);

                if (app.getUserCrt())
                {
                    float totalContentHeight = app.getUserCrt()->getNrObiecte() * 85.f;
                    float maxScroll = std::max(0.f, totalContentHeight - 650.f);
                    if (scrollOffset < 0) scrollOffset = 0;
                    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

                    float cy = 100.f - scrollOffset;
                    if (currentCat == Category::Security)
                    {
                        sf::Text info = createUtf8Text("Real-time analysis of your vault:", font, 18,
                                                       sf::Color(100, 100, 100));
                        info.setPosition({SIDE_W + 30, cy});
                        window.draw(info);
                        cy += 40;

                        std::vector<std::string> demoAlerts = {
                            "[WARN] Parola Facebook este slaba (< 12 caractere).",
                            "[INFO] Card Revolut expira in 2028.",
                            "[SECURE] 2 Identitati configurate corect."
                        };

                        for (const auto& alert : demoAlerts)
                        {
                            sf::Color c = (alert.find("WARN") != std::string::npos) ? CLR_ERR : sf::Color(40, 167, 69);
                            if (alert.find("INFO") != std::string::npos) c = CLR_ACCENT;

                            sf::Text tAlert = createUtf8Text(alert, font, 16, c);
                            tAlert.setPosition({SIDE_W + 30, cy});
                            window.draw(tAlert);
                            cy += 30;
                        }
                    }
                    else
                    {
                        for (int i = 0; ; ++i)
                        {
                            try
                            {
                                Seif* itm = app.getUserCrt()->getObiectAt(i);
                                bool match = (currentCat == Category::Accounts && itm->getTip() == "Date Autentificare")
                                    ||
                                    (currentCat == Category::Cards && itm->getTip() == "Card Bancar") ||
                                    (currentCat == Category::Notes && itm->getTip() == "Notita Securizata") ||
                                    (currentCat == Category::Identities && itm->getTip() == "Identitate");

                                if (match)
                                {
                                    if (cy > 50.f && cy < 800.f)
                                    {
                                        sf::RectangleShape card({800, 75});
                                        card.setPosition({SIDE_W + 30, cy});
                                        card.setFillColor(sf::Color::White);

                                        if (click && card.getGlobalBounds().contains(mPos))
                                        {
                                            activeItem = itm;
                                            appState = AppState::Edit;
                                            buffers.clear();
                                            cursors.clear();
                                            anchors.clear();
                                            focusKey = "eticheta";
                                            buffers["eticheta"] = toSfStr(itm->getEticheta());

                                            if (auto* da = dynamic_cast<DateAutentificare*>(itm))
                                            {
                                                buffers["user"] = toSfStr(da->getUtilizator());
                                                buffers["pass"] = toSfStr(da->getParola());
                                                buffers["url"] = toSfStr(da->getUrl());
                                                buffers["note"] = toSfStr(da->getNote());
                                            }
                                            else if (auto* cb = dynamic_cast<CardBancar*>(itm))
                                            {
                                                buffers["num"] = toSfStr(cb->getNumar());
                                                buffers["exp"] = toSfStr(cb->getDataExp());
                                                buffers["cvv"] = toSfStr(cb->getCVV());
                                            }
                                            else if (auto* ns = dynamic_cast<NotitaSecurizata*>(itm))
                                            {
                                                buffers["note"] = toSfStr(ns->getNotita());
                                            }
                                            else if (auto* id = dynamic_cast<Identitate*>(itm))
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
                                        }
                                        window.draw(card);
                                        sf::Text name = createUtf8Text(itm->getEticheta(), font, 19, sf::Color::Black);
                                        name.setPosition({SIDE_W + 60, cy + 25});
                                        window.draw(name);
                                    }
                                    cy += 85.f;
                                }
                            }
                            catch (...) { break; }
                        }
                    }
                }

                if (currentCat != Category::Security)
                {
                    sf::RectangleShape fab({160, 45});
                    fab.setPosition({1000, 810});
                    fab.setFillColor(CLR_ACCENT);
                    if (fab.getGlobalBounds().contains(mPos))
                    {
                        fab.setFillColor(sf::Color(0, 80, 200));
                    }

                    if ((click && fab.getGlobalBounds().contains(mPos)) || triggerSave)
                    {
                        appState = AppState::Add;
                        activeItem = nullptr;
                        buffers.clear();
                        cursors.clear();
                        anchors.clear();
                        focusKey = "eticheta";
                        triggerSave = false;
                    }
                    window.draw(fab);

                    sf::Text fabT = createUtf8Text("+ Add item", font, 18, sf::Color::White);
                    fabT.setPosition({1000 + 80.f - fabT.getGlobalBounds().size.x / 2.f, 810 + 10.f});
                    window.draw(fabT);
                }
            }
            else
            {
                // FORMULAR EDITARE
                std::vector<std::pair<std::string, std::string>> fields = {{"Eticheta / Title", "eticheta"}};

                if (currentCat == Category::Accounts)
                {
                    fields.insert(fields.end(), {
                                      {"Username", "user"},
                                      {"Password", "pass"},
                                      {"Website URL", "url"},
                                      {"Notes", "note"}
                                  });
                }
                else if (currentCat == Category::Cards)
                    fields.insert(fields.end(), {{"Card Number", "num"}, {"Expiry Date", "exp"}, {"CVV", "cvv"}});
                else if (currentCat == Category::Identities)
                    fields.insert(fields.end(), {
                                      {"First Name", "fn"}, {"Last Name", "ln"}, {"Phone", "tel"}, {"Email", "em"},
                                      {"Street", "str"}, {"City", "ors"}, {"County", "jud"}, {"Country", "tar"},
                                      {"Postal Code", "cp"}
                                  });
                else if (currentCat == Category::Notes) fields.push_back({"Note Content", "note"});

                float fy = 50.f;
                float step = (fields.size() > 5) ? 65.f : 90.f;

                for (auto& f : fields)
                {
                    sf::Text lab = createUtf8Text(f.first, font, 15, sf::Color(100, 100, 100));
                    lab.setPosition({SIDE_W + 40, fy});
                    window.draw(lab);

                    sf::RectangleShape bx({600, 40});
                    bx.setPosition({SIDE_W + 40, fy + 25});
                    bx.setFillColor(sf::Color::White);
                    bx.setOutlineThickness(focusKey == f.second ? 2.f : 1.f);
                    bx.setOutlineColor(focusKey == f.second ? CLR_ACCENT : sf::Color(200, 200, 200));
                    window.draw(bx);

                    sf::String disp = buffers[f.second];
                    if (f.second == "pass" && !showPass) disp = sf::String(std::string(disp.getSize(), '*'));

                    sf::Text txt(font, disp, 18);
                    txt.setPosition({SIDE_W + 50, fy + 31});
                    txt.setFillColor(sf::Color::Black);

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

                    if (f.second == "pass")
                    {
                        sf::RectangleShape showBtn({50, 25});
                        showBtn.setPosition({SIDE_W + 40 + 600 - 60, fy + 32});
                        showBtn.setFillColor(sf::Color(230, 230, 230));
                        if (click && showBtn.getGlobalBounds().contains(mPos)) showPass = !showPass;
                        window.draw(showBtn);
                        sf::Text sBtnT = createUtf8Text(showPass ? "Hide" : "Show", font, 12, sf::Color::Black);
                        sBtnT.setPosition({showBtn.getPosition().x + 8, showBtn.getPosition().y + 4});
                        window.draw(sBtnT);
                    }

                    if (focusKey == f.second && anchors[f.second] != cursors[f.second])
                    {
                        size_t s = std::min(anchors[f.second], cursors[f.second]);
                        size_t e = std::max(anchors[f.second], cursors[f.second]);
                        sf::RectangleShape sel({txt.findCharacterPos(e).x - txt.findCharacterPos(s).x, 24});
                        sel.setPosition({txt.findCharacterPos(s).x, fy + 28});
                        sel.setFillColor(CLR_SELECT);
                        window.draw(sel);
                    }
                    window.draw(txt);

                    if (focusKey == f.second && showCursor)
                    {
                        sf::RectangleShape cl({2, 24});
                        cl.setPosition({txt.findCharacterPos(cursors[f.second]).x, fy + 28});
                        cl.setFillColor(CLR_ACCENT);
                        window.draw(cl);
                    }
                    fy += step;
                }

                // --- BUTON BACK (Stil Outline - Modern) ---
                sf::RectangleShape bBack({160, 45});
                bBack.setPosition({820, 810}); // Mutat spre dreapta
                bBack.setFillColor(sf::Color::Transparent);
                bBack.setOutlineThickness(1.5f);
                bBack.setOutlineColor(sf::Color(180, 180, 180));

                if (click && bBack.getGlobalBounds().contains(mPos))
                {
                    appState = AppState::Vault;
                    guiError = "";
                    showPass = false;
                }
                window.draw(bBack);

                sf::Text tBack = createUtf8Text("Cancel", font, 18, sf::Color(120, 120, 120));
                // Centrare text în buton
                tBack.setPosition({820 + 80.f - tBack.getGlobalBounds().size.x / 2.f, 810 + 10.f});
                window.draw(tBack);

                // --- BUTON SAVE (Stil Proeminent) ---
                sf::RectangleShape bS({160, 45});
                bS.setPosition({1000, 810}); // În extremitatea dreaptă
                bS.setFillColor(CLR_ACCENT);
                // Efect de hover (opțional)
                if (bS.getGlobalBounds().contains(mPos)) bS.setFillColor(sf::Color(0, 80, 200));
                if ((click && bS.getGlobalBounds().contains(mPos)) || triggerSave)
                {
                    try
                    {
                        if (!app.getUserCrt()) throw EroareValidare("Sistem", "Utilizator nelogat!");
                        std::map<std::string, std::string> date;
                        for (auto const& [k, v] : buffers) date[k] = u8ToStr(v.toUtf8());

                        if (currentCat == Category::Accounts)
                        {
                            std::string p = date["pass"];
                            auto& cfg = Configuratie::getInstance();
                            if (p.length() < cfg.getLungimeMinimaParola())
                                throw EroareValidare(date["eticheta"], static_cast<int>(p.length()),
                                                     static_cast<int>(cfg.getLungimeMinimaParola()));
                            bool hasM = false;
                            bool hasS = false;
                            std::string sym = cfg.getCaractereSpecialeValide();
                            for (char c : p)
                            {
                                if (isupper(c)) hasM = true;
                                if (sym.find(c) != std::string::npos) hasS = true;
                            }
                            if (!hasM || !hasS)
                                throw EroareValidare(date["eticheta"],
                                                     "Parola trebuie să conțină o majuscula și un simbol.");
                        }

                        if (appState == AppState::Add)
                        {
                            if (currentCat == Category::Accounts)
                            {
                                date["platforma"] = date["eticheta"];
                                date["utilizator"] = date["user"];
                                date["parola"] = date["pass"];
                                date["url"] = date["url"];
                                date["note"] = date["note"];
                                app.adaugaObiectInSeif("DateAutentificare", date);
                            }
                            else if (currentCat == Category::Cards)
                            {
                                date["numar"] = date["num"];
                                date["dataExpirare"] = date["exp"];
                                date["cvv"] = date["cvv"];
                                app.adaugaObiectInSeif("CardBancar", date);
                            }
                            else if (currentCat == Category::Notes)
                            {
                                date["notita"] = date["note"];
                                app.adaugaObiectInSeif("NotitaSecurizata", date);
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
                                app.adaugaObiectInSeif("Identitate", date);
                            }
                        }
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
                        app.salveazaDatelePeDisc();
                        guiError = "";
                        showPass = false;
                        triggerSave = false;
                    }
                    catch (const ScriptException& e)
                    {
                        guiError = std::string(e.what()) + "\n" + e.getSugestie();
                    }
                }
                window.draw(bS);

                sf::Text tSave = createUtf8Text("Save Changes", font, 18, sf::Color::White);
                tSave.setPosition({1000 + 80.f - tSave.getGlobalBounds().size.x / 2.f, 810 + 10.f});
                window.draw(tSave);

                if (!guiError.empty())
                {
                    sf::Text err = createUtf8Text(guiError, font, 16, CLR_ERR);
                    err.setPosition({SIDE_W + 40, 750});
                    window.draw(err);
                }
            }

            window.display();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "CRITICAL ERROR: " << e.what() << std::endl;
#ifdef _WIN32
        MessageBoxA(nullptr, e.what(), "Critical Error", MB_ICONERROR);
#endif
        return -1;
    }
    catch (...)
    {
        std::cerr << "UNKNOWN CRITICAL ERROR!" << std::endl;
        return -2;
    }

    return 0;
}
