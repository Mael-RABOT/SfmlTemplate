#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/Config.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <cstddef>
#include <iostream>

#include "Theme.hpp"

namespace GUI {

class Widget;
class Window;
class Button;
class Label;
class Input;
class Panel;

namespace FontManager {
    sf::Font loadDefaultFont();
}

class Widget {
    protected:
        sf::Vector2f position;
        sf::Vector2f size;
        bool visible;
        bool enabled;
        std::string id;
        sf::RenderWindow* windowRef;
        Widget* parent;

    public:
        Widget(const sf::Vector2f& pos = {0, 0}, const sf::Vector2f& siz = {100, 30});
        virtual ~Widget() = default;

        virtual void draw(sf::RenderWindow& window) = 0;
        virtual void update(float deltaTime) = 0;
        virtual bool handleEvent(const sf::Event& event) = 0;

        void setPosition(const sf::Vector2f& pos) { position = pos; }
        void setSize(const sf::Vector2f& siz) { size = siz; }
        void setVisible(bool vis) { visible = vis; }
        void setEnabled(bool en) { enabled = en; }
        void setId(const std::string& widgetId) { id = widgetId; }
        void setWindowRef(sf::RenderWindow* window) { windowRef = window; }
        void setParent(Widget* p) { parent = p; }

        sf::Vector2f getPosition() const { return position; }
        sf::Vector2f getSize() const { return size; }
        bool isVisible() const { return visible; }
        bool isEnabled() const { return enabled; }
        std::string getId() const { return id; }
        Widget* getParent() const { return parent; }

        bool contains(const sf::Vector2f& point) const;
};

struct WindowConfig {
    // Basic window properties
    std::string title = "GUI Window";
    unsigned int width = 800;
    unsigned int height = 600;

    // Visual styling
    sf::Color backgroundColor = sf::Color::White;
    sf::Color borderColor = sf::Color::Black;
    unsigned int borderWidth = 0;

    // Window behavior
    bool resizable = true;
    bool closable = true;
    bool minimizable = true;
    bool maximizable = true;
    bool titlebar = true;
    bool fullscreen = false;

    // Performance settings
    unsigned int framerateLimit = 60;
    bool verticalSync = false;
    unsigned int antialiasingLevel = 0;

    // Window positioning
    bool centerOnScreen = true;
    int positionX = 100;
    int positionY = 100;

    float opacity = 1.0f; // 0.0 to 1.0
    sf::Image* icon = nullptr;

    std::uint32_t getStyleFlags() const {
        std::uint32_t style = sf::Style::None;

        if (titlebar) style |= sf::Style::Titlebar;
        if (resizable) style |= sf::Style::Resize;
        if (closable) style |= sf::Style::Close;

        if (style == sf::Style::None && !fullscreen) {
            style = sf::Style::Default;
        }

        return style;
    }
};

class Window {
    private:
        sf::RenderWindow window;
        std::vector<std::unique_ptr<Widget>> widgets;
        WindowConfig config;
        bool running;

    public:
        Window(const WindowConfig& windowConfig = WindowConfig{});

        ~Window() = default;

        void run();
        void close();
        bool isOpen() const { return window.isOpen(); }

        void addWidget(std::unique_ptr<Widget> widget);
        Widget* getWidget(const std::string& id);
        void removeWidget(const std::string& id);

        // Configuration getters/setters
        void setBackgroundColor(const sf::Color& color) { config.backgroundColor = color; }
        sf::Color getBackgroundColor() const { return config.backgroundColor; }

        void setOpacity(float opacity);
        float getOpacity() const { return config.opacity; }

        void setFramerateLimit(unsigned int limit);
        void setVerticalSync(bool enabled);

        // Window properties
        sf::RenderWindow& getRenderWindow() { return window; }
        const WindowConfig& getConfig() const { return config; }

        sf::Vector2f getSize() const {
            return sf::Vector2f(static_cast<float>(window.getSize().x),
                               static_cast<float>(window.getSize().y));
        }

        // Runtime configuration updates
        void updateConfig(const WindowConfig& newConfig);
        void setResizable(bool resizable);
        void setIcon(const sf::Image& icon);

    private:
        void applyConfig();
        void centerWindow();
};

class Button : public Widget {
    private:
        sf::RectangleShape shape;
        std::unique_ptr<sf::Text> text;
        sf::Font font;
        std::function<void()> onClick;
        bool hovered;
        bool pressed;

    public:
        Button(const std::string& label, const sf::Vector2f& pos = {0, 0}, const sf::Vector2f& siz = {100, 30});

        void draw(sf::RenderWindow& window) override;
        void update(float deltaTime) override;
        bool handleEvent(const sf::Event& event) override;

        void setOnClick(std::function<void()> callback) { onClick = callback; }
        void setText(const std::string& label);
        void setColor(const sf::Color& color);
        void setTextColor(const sf::Color& color);
};

class Label : public Widget {
    private:
        std::unique_ptr<sf::Text> text;
        sf::Font font;

    public:
        Label(const std::string& text, const sf::Vector2f& pos = {0, 0});

        void draw(sf::RenderWindow& window) override;
        void update(float deltaTime) override;
        bool handleEvent(const sf::Event& event) override;

        void setText(const std::string& newText);
        void setColor(const sf::Color& color);
        void setFontSize(unsigned int size);
};

class Input : public Widget {
    private:
        sf::RectangleShape shape;
        std::unique_ptr<sf::Text> text;
        sf::Font font;
        std::string value;
        bool focused;
        bool cursorVisible;
        float cursorBlinkTime;
        std::size_t cursorPosition;

    public:
        Input(const std::string& placeholder = "", const sf::Vector2f& pos = {0, 0}, const sf::Vector2f& siz = {200, 30});

        void draw(sf::RenderWindow& window) override;
        void update(float deltaTime) override;
        bool handleEvent(const sf::Event& event) override;

        void setValue(const std::string& newValue);
        std::string getValue() const { return value; }
        void setPlaceholder(const std::string& placeholder);
        void setColor(const sf::Color& color);
        void setTextColor(const sf::Color& color);
};

class Panel : public Widget {
    private:
        sf::RectangleShape shape;
        std::vector<std::unique_ptr<Widget>> children;

    public:
        Panel(const sf::Vector2f& pos = {0, 0}, const sf::Vector2f& siz = {300, 200});

        void draw(sf::RenderWindow& window) override;
        void update(float deltaTime) override;
        bool handleEvent(const sf::Event& event) override;

        void addChild(std::unique_ptr<Widget> widget);
        void setColor(const sf::Color& color);
};

} // namespace GUI
