#include "../../include/GUI.hpp"

namespace GUI {

Label::Label(const std::string& text, const sf::Vector2f& pos)
    : Widget(pos, {0, 0}) {

    font = FontManager::loadDefaultFont();

    this->text = std::make_unique<sf::Text>(font, text, 16);
    this->text->setFillColor(sf::Color::Black);
    this->text->setPosition(position);

    sf::FloatRect textBounds = this->text->getLocalBounds();
    size = sf::Vector2f(textBounds.size.x, textBounds.size.y);
}

void Label::draw(sf::RenderWindow& window) {
    if (!visible) return;
    window.draw(*text);
}

void Label::update([[maybe_unused]] float deltaTime) {
    // Labels don't need updates
}

bool Label::handleEvent([[maybe_unused]] const sf::Event& event) {
    // Labels don't handle events
    return false;
}

void Label::setText(const std::string& newText) {
    text->setString(newText);

    sf::FloatRect textBounds = text->getLocalBounds();
    size = sf::Vector2f(textBounds.size.x, textBounds.size.y);
}

void Label::setColor(const sf::Color& color) {
    text->setFillColor(color);
}

void Label::setFontSize(unsigned int size) {
    text->setCharacterSize(size);

    sf::FloatRect textBounds = text->getLocalBounds();
    this->size = sf::Vector2f(textBounds.size.x, textBounds.size.y);
}

} // namespace GUI
