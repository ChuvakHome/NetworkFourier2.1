
#include <Fourier/UI/TextPanel.hpp>

#include <cmath>
#include <Fourier/Global.hpp>

UITextPanel::UITextPanel(const std::string &text, float width, float height) : m_x(0), m_y(0), m_width(width), m_height(height), m_hovered(false), m_text(text) {
    float minWidth = text.length() * uiManager.getSymbolBounds().width;
    float minHeight = uiManager.getSymbolBounds().height;

    m_width = fmax(m_width, minWidth);
    m_height = fmax(m_height, minHeight);

    m_textPanelRect = sf::RectangleShape(sf::Vector2f(m_width, m_height));
    m_textPanelRect.setFillColor(sf::Color(0x60, 0x60, 0x60));
    m_textPanelRect.setOutlineThickness(1);
    m_textPanelRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
}

void UITextPanel::setSize(const sf::Vector2f &size) {
    m_width = size.x;
    m_height = size.y;

    float minWidth = m_text.length() * uiManager.getSymbolBounds().width;
    float minHeight = uiManager.getSymbolBounds().height;

    m_width = fmax(m_width, minWidth);
    m_height = fmax(m_height, minHeight);

    m_textPanelRect.setSize(sf::Vector2f(m_width, m_height));
}

sf::Vector2f UITextPanel::getSize() const {
    return sf::Vector2f(m_width, m_height);
}

void UITextPanel::onMouseMove(const sf::RenderWindow &window) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    m_hovered = m_textPanelRect.getGlobalBounds().contains(mousePosition);
}

void UITextPanel::onMouseClick(const sf::RenderWindow &window) {
    static_cast<void>(window);

    bool clicked = m_hovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    m_textPanelRect.setFillColor(clicked ? sf::Color(0x00, 0x00, 0x00) : sf::Color(0x60, 0x60, 0x60));
}

void UITextPanel::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;

    m_textPanelRect.setPosition(pos);
}

sf::Vector2f UITextPanel::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UITextPanel::draw(sf::RenderTarget& target, sf::RenderStates state) const {
    target.draw(m_textPanelRect, state);

    for (std::size_t i = 0; i < m_text.length(); ++i) {
        sf::Sprite charSprite = uiManager.getSpriteByCharacter(m_text[i]);
        charSprite.setPosition(sf::Vector2f(m_x + i * uiManager.getSymbolBounds().width, m_y));
        target.draw(charSprite, state);
    }
}
