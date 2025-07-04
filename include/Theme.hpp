#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <mutex>

namespace Theme {

    enum class ColorRole {
        PRIMARY, PRIMARY_DARK, PRIMARY_LIGHT,
        SECONDARY, SECONDARY_DARK, SECONDARY_LIGHT,
        BACKGROUND, BACKGROUND_DARK, BACKGROUND_LIGHT, SURFACE, SURFACE_VARIANT,
        TEXT_PRIMARY, TEXT_SECONDARY, TEXT_DISABLED, TEXT_ON_PRIMARY, TEXT_ON_SECONDARY,
        HIGHLIGHT, HOVER, PRESSED, FOCUS, SELECTED,
        SUCCESS, WARNING, ERROR, INFO,
        ACCENT_PURPLE, ACCENT_CYAN, ACCENT_PINK, ACCENT_ORANGE, ACCENT_GREEN, ACCENT_YELLOW,
        BORDER, BORDER_LIGHT, OUTLINE, DIVIDER,
        TRANSPARENT, OVERLAY, SHADOW
    };

    class DarkVibrantTheme {
        private:
            std::unordered_map<ColorRole, sf::Color> colors;

            DarkVibrantTheme() {
                initializeColors();
            }

            void initializeColors() {
                colors[ColorRole::PRIMARY] = sf::Color(0, 150, 255);
                colors[ColorRole::PRIMARY_DARK] = sf::Color(0, 100, 200);
                colors[ColorRole::PRIMARY_LIGHT] = sf::Color(100, 200, 255);
                colors[ColorRole::SECONDARY] = sf::Color(150, 50, 255);
                colors[ColorRole::SECONDARY_DARK] = sf::Color(100, 30, 200);
                colors[ColorRole::SECONDARY_LIGHT] = sf::Color(200, 150, 255);
                colors[ColorRole::BACKGROUND] = sf::Color(18, 18, 20);
                colors[ColorRole::BACKGROUND_DARK] = sf::Color(12, 12, 14);
                colors[ColorRole::BACKGROUND_LIGHT] = sf::Color(28, 28, 32);
                colors[ColorRole::SURFACE] = sf::Color(32, 32, 38);
                colors[ColorRole::SURFACE_VARIANT] = sf::Color(45, 45, 52);
                colors[ColorRole::TEXT_PRIMARY] = sf::Color(240, 240, 245);
                colors[ColorRole::TEXT_SECONDARY] = sf::Color(180, 180, 190);
                colors[ColorRole::TEXT_DISABLED] = sf::Color(120, 120, 130);
                colors[ColorRole::TEXT_ON_PRIMARY] = sf::Color(255, 255, 255);
                colors[ColorRole::TEXT_ON_SECONDARY] = sf::Color(255, 255, 255);
                colors[ColorRole::HIGHLIGHT] = sf::Color(0, 255, 150);
                colors[ColorRole::HOVER] = sf::Color(255, 255, 255, 30);
                colors[ColorRole::PRESSED] = sf::Color(255, 255, 255, 50);
                colors[ColorRole::FOCUS] = sf::Color(0, 150, 255, 100);
                colors[ColorRole::SELECTED] = sf::Color(0, 150, 255, 80);
                colors[ColorRole::SUCCESS] = sf::Color(0, 255, 100);
                colors[ColorRole::WARNING] = sf::Color(255, 180, 0);
                colors[ColorRole::ERROR] = sf::Color(255, 50, 80);
                colors[ColorRole::INFO] = sf::Color(0, 200, 255);
                colors[ColorRole::ACCENT_PURPLE] = sf::Color(200, 100, 255);
                colors[ColorRole::ACCENT_CYAN] = sf::Color(0, 255, 255);
                colors[ColorRole::ACCENT_PINK] = sf::Color(255, 100, 200);
                colors[ColorRole::ACCENT_ORANGE] = sf::Color(255, 120, 50);
                colors[ColorRole::ACCENT_GREEN] = sf::Color(100, 255, 150);
                colors[ColorRole::ACCENT_YELLOW] = sf::Color(255, 255, 0);
                colors[ColorRole::BORDER] = sf::Color(80, 80, 90);
                colors[ColorRole::BORDER_LIGHT] = sf::Color(120, 120, 130);
                colors[ColorRole::OUTLINE] = sf::Color(150, 150, 160);
                colors[ColorRole::DIVIDER] = sf::Color(50, 50, 60);
                colors[ColorRole::TRANSPARENT] = sf::Color(0, 0, 0, 0);
                colors[ColorRole::OVERLAY] = sf::Color(0, 0, 0, 150);
                colors[ColorRole::SHADOW] = sf::Color(0, 0, 0, 80);
            }

        public:
            DarkVibrantTheme(const DarkVibrantTheme&) = delete;
            DarkVibrantTheme& operator=(const DarkVibrantTheme&) = delete;

            static DarkVibrantTheme& getInstance() {
                static DarkVibrantTheme instance;
                return instance;
            }

            sf::Color getColor(ColorRole role) const {
                auto it = colors.find(role);
                return (it != colors.end()) ? it->second : sf::Color::Magenta;
            }

            sf::Color withAlpha(ColorRole role, std::uint8_t alpha) const {
                sf::Color color = getColor(role);
                color.a = alpha;
                return color;
            }

            sf::Color blend(ColorRole role1, ColorRole role2, float ratio = 0.5f) const {
                sf::Color c1 = getColor(role1);
                sf::Color c2 = getColor(role2);
                return sf::Color(
                    static_cast<std::uint8_t>(c1.r * (1.0f - ratio) + c2.r * ratio),
                    static_cast<std::uint8_t>(c1.g * (1.0f - ratio) + c2.g * ratio),
                    static_cast<std::uint8_t>(c1.b * (1.0f - ratio) + c2.b * ratio),
                    static_cast<std::uint8_t>(c1.a * (1.0f - ratio) + c2.a * ratio)
                );
            }

            sf::Color brighten(ColorRole role, float factor = 1.2f) const {
                sf::Color color = getColor(role);
                return sf::Color(
                    static_cast<std::uint8_t>(std::min(255.0f, color.r * factor)),
                    static_cast<std::uint8_t>(std::min(255.0f, color.g * factor)),
                    static_cast<std::uint8_t>(std::min(255.0f, color.b * factor)),
                    color.a
                );
            }

            sf::Color darken(ColorRole role, float factor = 0.8f) const {
                return brighten(role, factor);
            }
    };

    inline sf::Color color(ColorRole role) {
        return DarkVibrantTheme::getInstance().getColor(role);
    }

    namespace Utils {
        sf::Color fromHex(const std::string& hex);
        std::string toHex(const sf::Color& color);
        sf::Color lerp(const sf::Color& a, const sf::Color& b, float t);
        sf::Color getContrastingColor(const sf::Color& background);
    }

} // namespace Theme
