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

const sf::Color CLR_NAVY(5, 15, 35);
const sf::Color CLR_ACCENT(0, 106, 255);
const sf::Color CLR_BG(245, 246, 248);
const sf::Color CLR_ERR(220, 53, 69);
const sf::Color CLR_SELECT(0, 106, 255, 100);
const float SIDE_W = 240.f;

enum class AppState { Vault, Edit, Add };
enum class Category { Accounts, Identities, Cards, Notes, Security };

std::string u8ToStr(const sf::U8String& u8str) { return std::string(reinterpret_cast<const char*>(u8str.data()), u8str.size()); }
sf::String toSfStr(const std::string& s) { return sf::String::fromUtf8(s.begin(), s.end()); }
sf::Text createUtf8Text(const std::string& str, const sf::Font& font, unsigned int size, sf::Color color) {
    sf::Text text(font, sf::String::fromUtf8(str.begin(), str.end()), size);
    text.setFillColor(color);
    return text;
}

size_t getIndexAtMouse(const sf::Text& text, float mouseX) {
    float localX = mouseX - text.getPosition().x;
    if (localX <= 0) return 0;
    const sf::String& s = text.getString();
    for (size_t i = 0; i < s.getSize(); ++i) {
        if (localX < text.findCharacterPos(i + 1).x - text.getPosition().x) return i;
    }
    return s.getSize();
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    auto& app = GestionarParole::getInstance();
    auto& res = ResourceManager::Instance();
    // Fereastra 1200x900
    sf::RenderWindow window(sf::VideoMode({1200, 900}), "$.C.R.I.P.T. Password Manager");
    window.setFramerateLimit(60);
    sf::Font& font = res.getFont("FiraSans-Regular.ttf");

    try {
        Utilizator u("Andrei", "andrei@proiect.ro", "secret123");
        u.adaugaObiect(std::make_unique<DateAutentificare>("Facebook", "andrei.nituică", "ParolăValidă123!"));
        u.adaugaObiect(std::make_unique<CardBancar>("Revolut", "4556 1234 5678 0000", "12/28", "123"));
        u.adaugaObiect(std::make_unique<NotitaSecurizata>("Cod Poartă", "Acces: 9988#"));
        u.adaugaObiect(std::make_unique<Identitate>("Acasă", "Nițuică", "Andrei", "0720x", "a@m.ro", "Str. X", "Otopeni", "IF", "RO", "0771"));
        app.getManager().adauga(u);
        app.login("Andrei", "secret123");
    } catch (...) {}

    AppState appState = AppState::Vault;
    Category currentCat = Category::Accounts;
    Seif* activeItem = nullptr;
    std::map<std::string, sf::String> buffers;
    std::map<std::string, size_t> cursors, anchors;
    std::string focusKey = "", guiError = "";
    bool showPass = false, showCursor = true;
    sf::Clock blinkClock, doubleClickClock;

    while (window.isOpen()) {
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        bool click = false, dblClick = false;
        if (blinkClock.getElapsedTime().asSeconds() > 0.5f) { showCursor = !showCursor; blinkClock.restart(); }

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (appState != AppState::Vault) {
                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                    bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);
                    // CTRL+A pentru selectie integrala
                    if (key->code == sf::Keyboard::Key::A && ctrl) { if (!focusKey.empty()) { anchors[focusKey] = 0; cursors[focusKey] = buffers[focusKey].getSize(); } }
                    // Navigare prin TAB
                    if (key->code == sf::Keyboard::Key::Tab) {
                        std::vector<std::string> order = {"eticheta"};
                        if (currentCat == Category::Accounts) order.insert(order.end(), {"user", "pass"});
                        else if (currentCat == Category::Cards) order.insert(order.end(), {"num", "exp", "cvv"});
                        else if (currentCat == Category::Identities) order.insert(order.end(), {"fn", "ln", "tel", "em", "str", "ors", "jud", "tar", "cp"});
                        else if (currentCat == Category::Notes) order.push_back("note");
                        auto it = std::find(order.begin(), order.end(), focusKey);
                        focusKey = (it != order.end()) ? order[(std::distance(order.begin(), it) + 1) % order.size()] : order[0];
                        cursors[focusKey] = anchors[focusKey] = buffers[focusKey].getSize();
                    }
                    if (!focusKey.empty()) {
                        auto& cur = cursors[focusKey]; auto& anc = anchors[focusKey]; auto& buf = buffers[focusKey];
                        // CTRL + Sageti pentru navigare rapida
                        if (key->code == sf::Keyboard::Key::Left) { if (ctrl) cur = 0; else if (cur > 0) cur--; if (!shift) anc = cur; }
                        if (key->code == sf::Keyboard::Key::Right) { if (ctrl) cur = buf.getSize(); else if (cur < buf.getSize()) cur++; if (!shift) anc = cur; }
                        if (key->code == sf::Keyboard::Key::Delete) {
                            if (anc != cur) { size_t s = std::min(anc, cur), e = std::max(anc, cur); buf.erase(s, e - s); cur = anc = s; }
                            else if (cur < buf.getSize()) buf.erase(cur, 1);
                        }
                    }
                }
                if (const auto* text = event->getIf<sf::Event::TextEntered>()) {
                    bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                    if (!focusKey.empty() && text->unicode != '\t' && text->unicode != '\r' && !ctrl) {
                        auto& cur = cursors[focusKey]; auto& anc = anchors[focusKey]; auto& buf = buffers[focusKey];
                        if (text->unicode == '\b') {
                            if (anc != cur) { size_t s = std::min(anc, cur), e = std::max(anc, cur); buf.erase(s, e - s); cur = anc = s; }
                            else if (cur > 0) { buf.erase(cur - 1, 1); cur--; anc = cur; }
                        } else if (text->unicode >= 32) {
                            if (anc != cur) { size_t s = std::min(anc, cur), e = std::max(anc, cur); buf.erase(s, e - s); cur = anc = s; }
                            buf.insert(cur, text->unicode); cur++; anc = cur;
                        }
                    }
                }
            }
            if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>()) { if (m->button == sf::Mouse::Button::Left) { click = true; if (doubleClickClock.getElapsedTime().asSeconds() < 0.3f) dblClick = true; doubleClickClock.restart(); } }
        }

        window.clear(CLR_BG);
        // Sidebar Navy, inaltime 900
        sf::RectangleShape sb({SIDE_W, 900.f}); sb.setFillColor(CLR_NAVY); window.draw(sb);
        std::vector<std::pair<std::string, Category>> nav = {{"Accounts", Category::Accounts}, {"Identities", Category::Identities}, {"Credit cards", Category::Cards}, {"Notes", Category::Notes}, {"Security report", Category::Security}};
        float ny = 120.f;
        for (auto& n : nav) {
            sf::RectangleShape b({SIDE_W, 50.f}); b.setPosition({0, ny}); b.setFillColor((currentCat == n.second && appState == AppState::Vault) ? sf::Color(20, 40, 70) : sf::Color::Transparent);
            if (click && b.getGlobalBounds().contains(mPos)) { currentCat = n.second; appState = AppState::Vault; guiError = ""; cursors.clear(); anchors.clear(); showPass = false; }
            window.draw(b); sf::Text t = createUtf8Text(n.first, font, 18, (currentCat == n.second && appState == AppState::Vault) ? sf::Color::White : sf::Color(150, 160, 180));
            t.setPosition({50, ny + 12}); window.draw(t); ny += 55.f;
        }

        if (appState == AppState::Vault) {
            sf::Text title = createUtf8Text("My Vault", font, 26, sf::Color::Black); title.setPosition({SIDE_W + 30, 30}); window.draw(title);
            float cy = 100.f;
            if (app.getUserCrt()) {
                for (int i = 0; ; ++i) {
                    try {
                        Seif* itm = app.getUserCrt()->getObiectAt(i);
                        bool match = (currentCat == Category::Accounts && itm->getTip() == "Date Autentificare") || (currentCat == Category::Cards && itm->getTip() == "Card Bancar") || (currentCat == Category::Notes && itm->getTip() == "Notita Securizata") || (currentCat == Category::Identities && itm->getTip() == "Identitate");
                        if (match) {
                            sf::RectangleShape card({800, 75}); card.setPosition({SIDE_W + 30, cy}); card.setFillColor(sf::Color::White);
                            if (click && card.getGlobalBounds().contains(mPos)) {
                                activeItem = itm; appState = AppState::Edit; buffers.clear(); cursors.clear(); anchors.clear(); focusKey = "eticheta"; buffers["eticheta"] = toSfStr(itm->getEticheta());
                                if (auto* da = dynamic_cast<DateAutentificare*>(itm)) { buffers["user"] = toSfStr(da->getUtilizator()); buffers["pass"] = toSfStr(da->getParola()); }
                                else if (auto* cb = dynamic_cast<CardBancar*>(itm)) { buffers["num"] = toSfStr(cb->getNumar()); buffers["exp"] = toSfStr(cb->getDataExp()); buffers["cvv"] = toSfStr(cb->getCVV()); }
                                else if (auto* ns = dynamic_cast<NotitaSecurizata*>(itm)) { buffers["note"] = toSfStr(ns->getNotita()); }
                                else if (auto* id = dynamic_cast<Identitate*>(itm)) { buffers["fn"] = toSfStr(id->get_prenume()); buffers["ln"] = toSfStr(id->get_nume()); buffers["tel"] = toSfStr(id->get_telefon()); buffers["em"] = toSfStr(id->get_email()); buffers["str"] = toSfStr(id->get_strada()); buffers["ors"] = toSfStr(id->get_oras()); buffers["jud"] = toSfStr(id->get_judet()); buffers["tar"] = toSfStr(id->get_tara()); buffers["cp"] = toSfStr(id->get_cod_postal()); }
                            }
                            window.draw(card); sf::Text name = createUtf8Text(itm->getEticheta(), font, 19, sf::Color::Black); name.setPosition({SIDE_W + 60, cy + 25}); window.draw(name); cy += 85.f;
                        }
                    } catch (...) { break; }
                }
            }
            // Ascundem butonul FAB pentru Security
            if (currentCat != Category::Security) {
                sf::RectangleShape fab({180, 50}); fab.setPosition({930, 800}); fab.setFillColor(CLR_ACCENT);
                if (click && fab.getGlobalBounds().contains(mPos)) { appState = AppState::Add; activeItem = nullptr; buffers.clear(); cursors.clear(); anchors.clear(); focusKey = "eticheta"; }
                window.draw(fab); sf::Text fabT = createUtf8Text("+ Add item", font, 18, sf::Color::White); fabT.setPosition({965, 812}); window.draw(fabT);
            }
        } else {
            std::vector<std::pair<std::string, std::string>> fields = {{"Eticheta / Title", "eticheta"}};
            if (currentCat == Category::Accounts) fields.insert(fields.end(), {{"Username", "user"}, {"Password", "pass"}});
            else if (currentCat == Category::Cards) fields.insert(fields.end(), {{"Card Number", "num"}, {"Expiry Date", "exp"}, {"CVV", "cvv"}});
            else if (currentCat == Category::Identities) fields.insert(fields.end(), {{"First Name", "fn"}, {"Last Name", "ln"}, {"Phone", "tel"}, {"Email", "em"}, {"Street", "str"}, {"City", "ors"}, {"County", "jud"}, {"Country", "tar"}, {"Postal Code", "cp"}});
            else if (currentCat == Category::Notes) fields.push_back({"Note Content", "note"});

            float fy = 50.f; float step = (fields.size() > 5) ? 65.f : 90.f;
            for (auto& f : fields) {
                sf::Text lab = createUtf8Text(f.first, font, 15, sf::Color(100,100,100)); lab.setPosition({SIDE_W + 40, fy}); window.draw(lab);
                sf::RectangleShape bx({600, 40}); bx.setPosition({SIDE_W + 40, fy + 25}); bx.setFillColor(sf::Color::White); bx.setOutlineThickness(focusKey == f.second ? 2.f : 1.f); bx.setOutlineColor(focusKey == f.second ? CLR_ACCENT : sf::Color(200, 200, 200)); window.draw(bx);

                sf::String disp = buffers[f.second];
                if (f.second == "pass" && !showPass) disp = sf::String(std::string(disp.getSize(), '*'));

                sf::Text txt(font, disp, 18); txt.setPosition({SIDE_W + 50, fy + 31}); txt.setFillColor(sf::Color::Black);

                if (click && bx.getGlobalBounds().contains(mPos)) { focusKey = f.second; cursors[f.second] = anchors[f.second] = getIndexAtMouse(txt, mPos.x); if (dblClick) { anchors[f.second] = 0; cursors[f.second] = buffers[f.second].getSize(); } }

                // --- BUTON SHOW/HIDE PAROLA ---
                if (f.second == "pass") {
                    sf::RectangleShape showBtn({50, 25});
                    showBtn.setPosition({SIDE_W + 40 + 600 - 60, fy + 32});
                    showBtn.setFillColor(sf::Color(230, 230, 230));
                    if (click && showBtn.getGlobalBounds().contains(mPos)) showPass = !showPass;
                    window.draw(showBtn);
                    sf::Text sBtnT = createUtf8Text(showPass ? "Hide" : "Show", font, 12, sf::Color::Black);
                    sBtnT.setPosition({showBtn.getPosition().x + 8, showBtn.getPosition().y + 4});
                    window.draw(sBtnT);
                }

                if (focusKey == f.second && anchors[f.second] != cursors[f.second]) {
                    size_t s = std::min(anchors[f.second], cursors[f.second]), e = std::max(anchors[f.second], cursors[f.second]);
                    sf::RectangleShape sel({txt.findCharacterPos(e).x - txt.findCharacterPos(s).x, 24}); sel.setPosition({txt.findCharacterPos(s).x, fy + 28}); sel.setFillColor(CLR_SELECT); window.draw(sel);
                }
                window.draw(txt);
                if (focusKey == f.second && showCursor) { sf::RectangleShape cl({2, 24}); cl.setPosition({txt.findCharacterPos(cursors[f.second]).x, fy + 28}); cl.setFillColor(CLR_ACCENT); window.draw(cl); }
                fy += step;
            }

            // Buton Save Y: 810
            sf::RectangleShape bS({160, 50}); bS.setPosition({520, 810}); bS.setFillColor(CLR_ACCENT);
            if (click && bS.getGlobalBounds().contains(mPos)) {
                try {
                    if (!app.getUserCrt()) throw EroareValidare("Sistem", "Utilizator nelogat!");
                    std::map<std::string, std::string> date; for (auto const& [k, v] : buffers) date[k] = u8ToStr(v.toUtf8());

                    if (currentCat == Category::Accounts) {
                        std::string p = date["pass"]; auto& cfg = Configuratie::getInstance();
                        if (p.length() < cfg.getLungimeMinimaParola()) throw EroareValidare(date["eticheta"], (int)p.length(), (int)cfg.getLungimeMinimaParola());
                        bool hasM = false, hasS = false; std::string sym = cfg.getCaractereSpecialeValide();
                        for(char c : p) { if(isupper(c)) hasM = true; if(sym.find(c) != std::string::npos) hasS = true; }
                        if (!hasM || !hasS) throw EroareValidare(date["eticheta"], "Parola trebuie să conțină o majuscula și un simbol.");
                    }

                    if (appState == AppState::Add) {
                        if (currentCat == Category::Accounts) { date["platforma"] = date["eticheta"]; date["utilizator"] = date["user"]; date["parola"] = date["pass"]; app.adaugaObiectInSeif("DateAutentificare", date); }
                        else if (currentCat == Category::Cards) { date["numar"] = date["num"]; date["dataExpirare"] = date["exp"]; date["cvv"] = date["cvv"]; app.adaugaObiectInSeif("CardBancar", date); }
                        else if (currentCat == Category::Notes) { date["notita"] = date["note"]; app.adaugaObiectInSeif("NotitaSecurizata", date); }
                        else if (currentCat == Category::Identities) {
                            date["nume"] = date["ln"]; date["prenume"] = date["fn"]; date["telefon"] = date["tel"]; date["email"] = date["em"];
                            date["strada"] = date["str"]; date["oras"] = date["ors"]; date["judet"] = date["jud"];
                            date["tara"] = date["tar"]; date["codPostal"] = date["cp"];
                            app.adaugaObiectInSeif("Identitate", date);
                        }
                    } else if (activeItem) {
                        activeItem->setEticheta(date["eticheta"]);
                        if (auto* da = dynamic_cast<DateAutentificare*>(activeItem)) { da->setter_parola(date["pass"]); da->setter_numeUtilizator(date["user"]); }
                        else if (auto* cb = dynamic_cast<CardBancar*>(activeItem)) { cb->setNumar(date["num"]); cb->setDataExp(date["exp"]); cb->setCVV(date["cvv"]); }
                        else if (auto* ns = dynamic_cast<NotitaSecurizata*>(activeItem)) { ns->set_notita(date["note"]); }
                        else if (auto* id = dynamic_cast<Identitate*>(activeItem)) {
                            // Sincronizare chei fixata
                            id->set_nume(date["ln"]); id->set_prenume(date["fn"]); id->set_telefon(date["tel"]); id->set_email(date["em"]);
                            id->set_strada(date["str"]); id->set_oras(date["ors"]); id->set_judet(date["jud"]);
                            id->set_tara(date["tar"]); id->set_cod_postal(date["cp"]);
                        }
                    }
                    appState = AppState::Vault; guiError = ""; showPass = false;
                } catch (const ScriptException& e) { guiError = std::string(e.what()) + "\n" + e.getSugestie(); }
            }
            window.draw(bS); sf::Text sT = createUtf8Text("Save", font, 22, sf::Color::White); sT.setPosition({570, 820}); window.draw(sT);
            if (!guiError.empty()) { sf::Text err = createUtf8Text(guiError, font, 16, CLR_ERR); err.setPosition({SIDE_W + 40, 750}); window.draw(err); }
        }
        window.display();
    }
    return 0;
}