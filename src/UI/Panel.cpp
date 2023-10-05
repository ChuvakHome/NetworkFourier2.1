
#include <Fourier/UI/Panel.hpp>

UIPanel::UIPanel(float width, float height, sf::Color color) : m_x(0), m_y(0), m_width(width), m_height(height) {
    m_panelRect = sf::RectangleShape(sf::Vector2f(m_width, m_height));
    // _panelRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));
    m_panelRect.setFillColor(color);
    m_panelRect.setOutlineThickness(1);
    m_panelRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
}

float UIPanel::getWidth() const {
    return m_width;
}

float UIPanel::getHeight() const {
    return m_height;
}

void UIPanel::setSize(const sf::Vector2f &size) {
    m_width = size.x;
    m_height = size.y;

    m_panelRect.setSize(size);
}

sf::Vector2f UIPanel::getSize() const {
    return sf::Vector2f(m_width, m_height);
}

void UIPanel::setBackgroundColor(sf::Color color) {
    m_panelRect.setFillColor(color);
}

void UIPanel::addUIElement(UIBase &uiBase, const sf::Vector2f &pos) {
    struct UIElementInfo info = {
        uiBase,
        pos.x,
        pos.y
    };

    m_elements.push_back(info);
}

void UIPanel::setPositionWithChildren(const sf::Vector2f &pos) {
    setPosition(pos);

    for (struct UIElementInfo &info: m_elements) {
        sf::Vector2f vec(pos.x + info.posX, pos.y + info.posY);
        info.uiBase.setPosition(vec);
    }
}

void UIPanel::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;

    m_panelRect.setPosition(pos);
}

sf::Vector2f UIPanel::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

sf::FloatRect UIPanel::getGlobalBounds() const {
    return sf::FloatRect(m_x, m_y, m_width, m_height);
}

void UIPanel::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    target.draw(m_panelRect, state);

    for (const struct UIElementInfo &info: m_elements)
        target.draw(info.uiBase, state);
}
