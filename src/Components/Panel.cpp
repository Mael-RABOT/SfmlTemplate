#include "../../include/GUI.hpp"

namespace GUI {

Panel::Panel(const sf::Vector2f& pos, const sf::Vector2f& siz)
    : Widget(pos, siz) {

    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(sf::Color(240, 240, 240));
    shape.setOutlineColor(sf::Color(180, 180, 180));
    shape.setOutlineThickness(1);
}

void Panel::draw(sf::RenderWindow& window) {
    if (!visible) return;

    window.draw(shape);

    for (auto& child : children) {
        if (child->isVisible()) {
            child->draw(window);
        }
    }
}

void Panel::update(float deltaTime) {
    if (!enabled) return;

    for (auto& child : children) {
        if (child->isVisible()) {
            child->update(deltaTime);
        }
    }
}

bool Panel::handleEvent(const sf::Event& event) {
    if (!enabled) return false;

    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->isVisible() && (*it)->isEnabled()) {
            if ((*it)->handleEvent(event)) {
                return true;
            }
        }
    }

    return false;
}

void Panel::addChild(std::unique_ptr<Widget> widget) {
    widget->setWindowRef(windowRef);
    widget->setParent(this);
    children.push_back(std::move(widget));
}

void Panel::setColor(const sf::Color& color) {
    shape.setFillColor(color);
}

} // namespace GUI
