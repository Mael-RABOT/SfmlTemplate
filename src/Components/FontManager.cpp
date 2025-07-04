#include "../../include/GUI.hpp"
#include "../../include/Exceptions.hpp"
#include <vector>
#include <string>

namespace GUI {

namespace FontManager {

    sf::Font loadDefaultFont() {
        sf::Font font;

        std::vector<std::string> fontPaths = {
            // Linux - prioritize fonts that are commonly available
            "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/Adwaita/AdwaitaSans-Regular.ttf",
            "/usr/share/fonts/TTF/FiraSansCondensed-Regular.ttf",
            "/usr/share/fonts/TTF/FiraSans-Regular.ttf",

            // Alternative DejaVu paths (in case they exist)
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",

            // macOS
            "/System/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "/Library/Fonts/Arial.ttf",

            // Windows (or if running under Wine or similar)
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf",

            // Additional fallbacks
            "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf"
        };

        for (const auto& path : fontPaths) {
            if (font.openFromFile(path)) {
                return font;
            }
        }

        GUI_THROW("Failed to load default font", GUIException::ErrorCode::FONT_ERROR, GUIException::Severity::ERROR);
    }

} // namespace FontManager

} // namespace GUI
