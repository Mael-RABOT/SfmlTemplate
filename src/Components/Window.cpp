#include "../../include/GUI.hpp"
#include <algorithm>

namespace GUI {

Window::Window(const WindowConfig& windowConfig)
    : config(windowConfig), running(false) {
    
    sf::VideoMode videoMode;
    if (config.fullscreen) {
        videoMode = sf::VideoMode::getDesktopMode();
    } else {
        videoMode = sf::VideoMode(sf::Vector2u(config.width, config.height));
    }

    window.create(videoMode, config.title, config.getStyleFlags());
    
    applyConfig();
}

void Window::applyConfig() {
    if (config.verticalSync) {
        window.setVerticalSyncEnabled(true);
    } else {
        window.setFramerateLimit(config.framerateLimit);
    }
    
    if (config.centerOnScreen && !config.fullscreen) {
        centerWindow();
    } else if (!config.fullscreen) {
        window.setPosition(sf::Vector2i(config.positionX, config.positionY));
    }

//    if (config.icon) {
//        window.setIcon(config.icon->getSize().x, config.icon->getSize().y,
//                      config.icon->getPixelsPtr());
//    }
}

void Window::centerWindow() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2u windowSize = window.getSize();
    
    int posX = (desktop.size.x - windowSize.x) / 2;
    int posY = (desktop.size.y - windowSize.y) / 2;
    
    window.setPosition(sf::Vector2i(posX, posY));
}

void Window::run() {
    running = true;
    sf::Clock clock;

    while (running && window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                running = false;
                break;
            }

            for (auto& widget : widgets) {
                if (widget->isVisible() && widget->isEnabled()) {
                    if (widget->handleEvent(*event)) {
                        break;
                    }
                }
            }
        }

        for (auto& widget : widgets) {
            if (widget->isVisible()) {
                widget->update(deltaTime);
            }
        }

        window.clear(config.backgroundColor);

        // Draw border if specified
        if (config.borderWidth > 0) {
            sf::RectangleShape border(sf::Vector2f(
                static_cast<float>(window.getSize().x), 
                static_cast<float>(window.getSize().y)
            ));
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineColor(config.borderColor);
            border.setOutlineThickness(static_cast<float>(config.borderWidth));
            window.draw(border);
        }

        for (auto& widget : widgets) {
            if (widget->isVisible()) {
                widget->draw(window);
            }
        }

        window.display();
    }
}

void Window::close() {
    running = false;
    window.close();
}

void Window::addWidget(std::unique_ptr<Widget> widget) {
    widget->setWindowRef(&window);
    widgets.push_back(std::move(widget));
}

Widget* Window::getWidget(const std::string& id) {
    for (auto& widget : widgets) {
        if (widget->getId() == id) {
            return widget.get();
        }
    }
    return nullptr;
}

void Window::removeWidget(const std::string& id) {
    widgets.erase(
        std::remove_if(widgets.begin(), widgets.end(),
            [&id](const std::unique_ptr<Widget>& widget) {
                return widget->getId() == id;
            }),
        widgets.end()
    );
}

void Window::setOpacity(float opacity) {
    config.opacity = std::max(0.0f, std::min(1.0f, opacity));
}

void Window::setFramerateLimit(unsigned int limit) {
    config.framerateLimit = limit;
    if (!config.verticalSync) {
        window.setFramerateLimit(limit);
    }
}

void Window::setVerticalSync(bool enabled) {
    config.verticalSync = enabled;
    window.setVerticalSyncEnabled(enabled);
    if (!enabled) {
        window.setFramerateLimit(config.framerateLimit);
    }
}

void Window::updateConfig(const WindowConfig& newConfig) {
    config = newConfig;

    bool needsRecreate = (config.width != newConfig.width || 
                         config.height != newConfig.height ||
                         config.fullscreen != newConfig.fullscreen ||
                         config.resizable != newConfig.resizable ||
                         config.titlebar != newConfig.titlebar);
    
    if (needsRecreate) {
        sf::VideoMode videoMode;
        if (config.fullscreen) {
            videoMode = sf::VideoMode::getDesktopMode();
        } else {
            videoMode = sf::VideoMode(sf::Vector2u(config.width, config.height));
        }
        
        window.create(videoMode, config.title, config.getStyleFlags());
    }
    
    applyConfig();
}

void Window::setResizable(bool resizable) {
    if (config.resizable != resizable) {
        config.resizable = resizable;
        updateConfig(config);
    }
}

void Window::setIcon(const sf::Image& icon [[maybe_unused]]) {
    // window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}

} // namespace GUI
