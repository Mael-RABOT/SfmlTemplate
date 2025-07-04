#include "../../include/GUI.hpp"

namespace GUI {

Button::Button(const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& siz)
    : Widget(pos, siz), hovered(false), pressed(false) {

    font = FontManager::loadDefaultFont();

    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(sf::Color(200, 200, 200));
    shape.setOutlineColor(sf::Color(100, 100, 100));
    shape.setOutlineThickness(2);

    text = std::make_unique<sf::Text>(font, label, 16);
    text->setFillColor(sf::Color::Black);

    sf::FloatRect textBounds = text->getLocalBounds();
    text->setPosition(sf::Vector2f(
        position.x + (size.x - textBounds.size.x) / 2,
        position.y + (size.y - textBounds.size.y) / 2 - 5
    ));
}

void Button::draw(sf::RenderWindow& window) {
    if (!visible) return;

    sf::Vector2f absolutePos = position;
    Widget* currentParent = parent;
    while (currentParent) {
        absolutePos += currentParent->getPosition();
        currentParent = currentParent->getParent();
    }

    shape.setPosition(absolutePos);

    if (pressed) {
        shape.setFillColor(sf::Color(150, 150, 150));
    } else if (hovered) {
        shape.setFillColor(sf::Color(220, 220, 220));
    } else {
        shape.setFillColor(sf::Color(200, 200, 200));
    }

    window.draw(shape);

    sf::FloatRect textBounds = text->getLocalBounds();
    text->setPosition(sf::Vector2f(
        absolutePos.x + (size.x - textBounds.size.x) / 2,
        absolutePos.y + (size.y - textBounds.size.y) / 2 - 5
    ));
    window.draw(*text);
}

void Button::update([[maybe_unused]] float deltaTime) {
    if (!enabled) return;
}

bool Button::handleEvent(const sf::Event& event) {
    if (!enabled) return false;

    if (event.is<sf::Event::MouseMoved>()) {
        if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
            sf::Vector2i mousePos;
            if (windowRef) {
                mousePos = sf::Mouse::getPosition(*windowRef);
            } else {
                mousePos = mouseMoved->position;
            }

            sf::Vector2f localMousePos = sf::Vector2f(mousePos);
            if (parent) {
                localMousePos -= parent->getPosition();
            }

            hovered = (localMousePos.x >= position.x && localMousePos.x <= position.x + size.x &&
                      localMousePos.y >= position.y && localMousePos.y <= position.y + size.y);
        }
    } else if (event.is<sf::Event::MouseButtonPressed>()) {
        if (auto mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos;
                if (windowRef) {
                    mousePos = sf::Mouse::getPosition(*windowRef);
                } else {
                    mousePos = mouseButton->position;
                }

                sf::Vector2f localMousePos = sf::Vector2f(mousePos);
                if (parent) {
                    localMousePos -= parent->getPosition();
                }

                hovered = (localMousePos.x >= position.x && localMousePos.x <= position.x + size.x &&
                          localMousePos.y >= position.y && localMousePos.y <= position.y + size.y);
                if (hovered) {
                    pressed = true;
                    return true;
                }
            }
        }
    } else if (event.is<sf::Event::MouseButtonReleased>()) {
        if (auto mouseButton = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseButton->button == sf::Mouse::Button::Left && pressed) {
                pressed = false;
                if (hovered && onClick) {
                    onClick();
                }
                return true;
            }
        }
    }

    return false;
}

void Button::setText(const std::string& label) {
    text->setString(label);

    sf::Vector2f absolutePos = position;
    Widget* currentParent = parent;
    while (currentParent) {
        absolutePos += currentParent->getPosition();
        currentParent = currentParent->getParent();
    }

    sf::FloatRect textBounds = text->getLocalBounds();
    text->setPosition(sf::Vector2f(
        absolutePos.x + (size.x - textBounds.size.x) / 2,
        absolutePos.y + (size.y - textBounds.size.y) / 2 - 5
    ));
}

void Button::setColor(const sf::Color& color) {
    shape.setFillColor(color);
}

void Button::setTextColor(const sf::Color& color) {
    text->setFillColor(color);
}

} // namespace GUI
