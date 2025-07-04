#include "../../include/GUI.hpp"

namespace GUI {

Input::Input(const std::string& placeholder, const sf::Vector2f& pos, const sf::Vector2f& siz)
    : Widget(pos, siz), value(""), focused(false), cursorVisible(true), cursorBlinkTime(0), cursorPosition(0) {

    font = FontManager::loadDefaultFont();

    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineColor(sf::Color(100, 100, 100));
    shape.setOutlineThickness(2);

    text = std::make_unique<sf::Text>(font, placeholder, 16);
    text->setFillColor(sf::Color(150, 150, 150));
    text->setPosition(sf::Vector2f(position.x + 5, position.y + (size.y - 16) / 2));
}

void Input::draw(sf::RenderWindow& window) {
    if (!visible) return;

    if (focused) {
        shape.setOutlineColor(sf::Color(0, 120, 255));
    } else {
        shape.setOutlineColor(sf::Color(100, 100, 100));
    }

    window.draw(shape);

    if (value.empty() && !focused) {
        text->setFillColor(sf::Color(150, 150, 150));
        text->setString(text->getString());
    } else {
        text->setFillColor(sf::Color::Black);
        text->setString(value);

        if (focused && cursorVisible) {
            sf::Text cursorText(font, "|", 16);
            cursorText.setFillColor(sf::Color::Black);

            sf::Text tempText(font, value.substr(0, cursorPosition), 16);

            float cursorX = position.x + 5 + tempText.getLocalBounds().size.x;
            cursorText.setPosition(sf::Vector2f(cursorX, position.y + (size.y - 16) / 2));

            window.draw(cursorText);
        }
    }

    window.draw(*text);
}

void Input::update(float deltaTime) {
    if (!enabled) return;

    if (focused) {
        cursorBlinkTime += deltaTime;
        if (cursorBlinkTime >= 0.5f) {
            cursorVisible = !cursorVisible;
            cursorBlinkTime = 0;
        }
    } else {
        cursorVisible = false;
        cursorBlinkTime = 0;
    }
}

bool Input::handleEvent(const sf::Event& event) {
    if (!enabled) return false;

    if (event.is<sf::Event::MouseButtonPressed>()) {
        if (auto mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos;
                if (windowRef) {
                    mousePos = sf::Mouse::getPosition(*windowRef);
                } else {
                    mousePos = mouseButton->position;
                }
                focused = contains(sf::Vector2f(mousePos));
                if (focused) {
                    cursorPosition = value.length();
                    cursorVisible = true;
                    cursorBlinkTime = 0;
                }
                return focused;
            }
        }
    } else if (event.is<sf::Event::TextEntered>()) {
        if (focused) {
            if (auto textEntered = event.getIf<sf::Event::TextEntered>()) {
                if (textEntered->unicode == 8) { // Backspace
                    if (cursorPosition > 0) {
                        value.erase(cursorPosition - 1, 1);
                        cursorPosition--;
                    }
                } else if (textEntered->unicode == 13) { // Enter
                    focused = false;
                } else if (textEntered->unicode >= 32 && textEntered->unicode < 128) { // Printable characters
                    value.insert(cursorPosition, 1, static_cast<char>(textEntered->unicode));
                    cursorPosition++;
                }
                cursorVisible = true;
                cursorBlinkTime = 0;
                return true;
            }
        }
    } else if (event.is<sf::Event::KeyPressed>()) {
        if (focused) {
            if (auto keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Left:
                        if (cursorPosition > 0) cursorPosition--;
                        cursorVisible = true;
                        cursorBlinkTime = 0;
                        return true;

                    case sf::Keyboard::Key::Right:
                        if (cursorPosition < value.length()) cursorPosition++;
                        cursorVisible = true;
                        cursorBlinkTime = 0;
                        return true;

                    case sf::Keyboard::Key::Home:
                        cursorPosition = 0;
                        cursorVisible = true;
                        cursorBlinkTime = 0;
                        return true;

                    case sf::Keyboard::Key::End:
                        cursorPosition = value.length();
                        cursorVisible = true;
                        cursorBlinkTime = 0;
                        return true;

                    case sf::Keyboard::Key::Delete:
                        if (cursorPosition < value.length()) {
                            value.erase(cursorPosition, 1);
                        }
                        return true;
                    default:
                        break;
                }
            }
        }
    }

    return false;
}

void Input::setValue(const std::string& newValue) {
    value = newValue;
    cursorPosition = value.length();
}

void Input::setPlaceholder(const std::string& placeholder) {
    if (value.empty() && !focused) {
        text->setString(placeholder);
    }
}

void Input::setColor(const sf::Color& color) {
    shape.setFillColor(color);
}

void Input::setTextColor(const sf::Color& color) {
    text->setFillColor(color);
}

} // namespace GUI
