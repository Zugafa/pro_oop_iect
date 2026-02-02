#include "ResourceManager.hpp"
#include "EroareFisier.h"
#include <iostream>

/**
 * @brief Returns the singleton instance of ResourceManager.
 */
ResourceManager& ResourceManager::Instance()
{
    static ResourceManager instance;
    return instance;
}

/**
 * @brief Default constructor preloading logo and fonts.
 */
ResourceManager::ResourceManager()
{
    loadTexture("images", "logo.png");
    loadFont("fonts", "FiraSans-Regular.ttf");

    loadTexture("images", "icon_menu.png"); // Menu button
    loadTexture("images", "icon_copy.png"); // Copy user, Copy password
    loadTexture("images", "icon_globe.png"); // Open Website
    loadTexture("images", "icon_edit.png"); // Edit
    loadTexture("images", "icon_trash.png"); // Delete
    loadTexture("images", "icon_ochiD.png"); // Open Eye (Show)
    loadTexture("images", "icon_ochiI.png"); // Closed Eye (Hide)
    loadTexture("images", "icon_search.png"); // Magnifier
    loadTexture("images", "icon_sort.png");   // Sort
    loadTexture("images", "icon_check.png");  // Checkmark for toggles
    loadTexture("images", "icon_beforeCheck.png"); // Unchecked state for toggles
    loadTexture("images", "icon_renew.png");  // Regenerate password
}

/**
 * @brief Loads a texture from file and stores it.
 */
void ResourceManager::loadTexture(const std::string& path, const std::string& texture_name)
{
    sf::Texture texture;

    std::string fullPath = path + "/" + texture_name;

    if (!texture.loadFromFile(fullPath))
        throw EroareFisier(fullPath);

    m_textures[texture_name] = texture;
    std::cout << "Loaded texture: " << texture_name << "\n";
}

/**
 * @brief Loads a font from file and stores it.
 */
void ResourceManager::loadFont(const std::string& path, const std::string& font_name)
{
    sf::Font font;
    std::string fullPath = path + "/" + font_name;

    if (!font.openFromFile(fullPath))
        throw EroareFisier(fullPath);

    m_fonts[font_name] = font;
    std::cout << "Loaded font: " << font_name << "\n";
}

/**
 * @brief Retrieves a texture by name.
 */
sf::Texture& ResourceManager::getTexture(const std::string& texture_name)
{
    if (!m_textures.contains(texture_name))
        throw std::runtime_error("Texture " + texture_name + " was not found.");

    return m_textures[texture_name];
}

/**
 * @brief Retrieves a font by name.
 */
sf::Font& ResourceManager::getFont(const std::string& font_name)
{
    if (!m_fonts.contains(font_name))
        throw std::runtime_error("Font " + font_name + " was not found.");
    return m_fonts[font_name];
}
