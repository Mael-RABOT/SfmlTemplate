#include "../include/GUI.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

int main() {
    try {
        GUI::WindowConfig config;
        config.title = "Title";
        config.width = 1200;
        config.height = 800;
        config.backgroundColor = Theme::DarkVibrantTheme::getInstance().getColor(Theme::ColorRole::BACKGROUND);
        config.borderColor = sf::Color::Blue;
        config.borderWidth = 2;
        config.resizable = false;
        config.centerOnScreen = true;
        config.framerateLimit = 120;
        config.antialiasingLevel = 8;

        GUI::Window window(config);

        auto button = std::make_unique<GUI::Button>("smth", sf::Vector2f((window.getSize().x / 2) - 60, (window.getSize().y / 2) - 20), sf::Vector2f(120, 40));
        button->setId("smth_button");
        button->setOnClick([]() {
            std::cout << "Action::smth" << std::endl;
        });
        window.addWidget(std::move(button));

        window.run();
    } catch (const GUI::GUIException& e) {
        std::cerr << "\033[1;31mGUI Exception:\033[0m " << e.getFormattedMessage() << std::endl;
        std::cerr << "\033[1;33mError Code:\033[0m " << e.getErrorCodeString() << std::endl;
        std::cerr << "\033[1;34mSeverity:\033[0m " << e.getSeverityString() << std::endl;
        std::cerr << "\033[1;36mStack Trace:\033[0m" << std::endl;
        for (const auto& frame : e.getStackTrace()) {
            std::cerr << "  " << frame << std::endl;
        }
        std::cerr << "\033[1;35mCustom Data:\033[0m" << std::endl;
        for (const auto& pair : e.getAllCustomData()) {
            std::cerr << "  " << pair.first << ": " << pair.second << std::endl;
        }
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
