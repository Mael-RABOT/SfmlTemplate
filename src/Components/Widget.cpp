#include "../../include/GUI.hpp"

namespace GUI {

Widget::Widget(const sf::Vector2f& pos, const sf::Vector2f& siz)
    : position(pos), size(siz), visible(true), enabled(true), windowRef(nullptr), parent(nullptr) {
}

bool Widget::contains(const sf::Vector2f& point) const {
    sf::Vector2f absolutePos = position;
    Widget* currentParent = parent;
    while (currentParent) {
        absolutePos += currentParent->getPosition();
        currentParent = currentParent->getParent();
    }

    return point.x >= absolutePos.x && point.x <= absolutePos.x + size.x &&
           point.y >= absolutePos.y && point.y <= absolutePos.y + size.y;
}

} // namespace GUI
