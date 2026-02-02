//
// Created by bgd on 12/15/2024.
//

#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>

/*
 *  Pentru ușurință clasa `ResourceManager` va fi un Singleton.
 */

//////////////////////////////////////////
////
/// << OBSERVAȚIE >> Fără atribute statice la sf::Texture, sf::Font, sf::SoundBuffer, etc. -> Segmentation Fault
///
///////////////////////////////////////////

/**
 * @class ResourceManager
 * @brief Singleton class for managing SFML resources such as textures and fonts.
 * 
 * Ensures that resources are loaded once and shared across the application,
 * avoiding redundant disk access and memory overhead.
 */
class ResourceManager {
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    /**
     * @brief Returns the single instance of the ResourceManager.
     * @return Reference to the singleton instance.
     */
    static ResourceManager& Instance();

    /**
     * @brief Retrieves a texture by name.
     * @param texture_name The name of the texture.
     * @return Reference to the sf::Texture object.
     * @throws std::runtime_error if the texture is not found.
     */
    sf::Texture& getTexture(const std::string& texture_name);

    /**
     * @brief Retrieves a font by name.
     * @param font_name The name of the font.
     * @return Reference to the sf::Font object.
     * @throws std::runtime_error if the font is not found.
     */
    sf::Font& getFont(const std::string& font_name);

private:
    /**
     * @brief Private constructor that preloads default resources.
     */
    ResourceManager();
    ~ResourceManager() = default;

    /**
     * @brief Loads a texture from a file.
     * @param path Directory path.
     * @param texture_name File name of the texture.
     */
    void loadTexture(const std::string& path, const std::string& texture_name);

    /**
     * @brief Loads a font from a file.
     * @param path Directory path.
     * @param font_name File name of the font.
     */
    void loadFont(const std::string& path, const std::string& font_name);

    std::unordered_map<std::string, sf::Texture> m_textures; ///< Storage for textures
    std::unordered_map<std::string, sf::Font> m_fonts;       ///< Storage for fonts
};



#endif //RESOURCEMANAGER_HPP
