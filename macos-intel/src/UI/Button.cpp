
#include <Fourier/UI/Button.hpp>

#include <iostream>

#include <Fourier/Global.hpp>
#include <SFML/Graphics.hpp>

UIButton::UIButton(const std::string &text, buttonActionPerformer performer): m_text(text), m_x(0), m_y(0), m_width((16 + 1) * uiManager.getSymbolBounds().width), m_height(uiManager.getSymbolBounds().width * 2), m_hovered(false), m_buttonActionPerformer(performer) {
    m_buttonRect = sf::RectangleShape(sf::Vector2f(m_width, m_height));
    m_buttonRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));
    m_buttonRect.setOutlineThickness(1);
    m_buttonRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
}

float UIButton::getWidth() const {
    return m_width;
}

float UIButton::getHeight() const {
    return m_height;
}

void UIButton::setSize(const sf::Vector2f &size) {
    m_width = size.x;
    m_height = size.y;

    std::cout << "Button size changed!" << std::endl;

    m_buttonRect.setSize(size);
}

sf::Vector2f UIButton::getSize() const {
    return sf::Vector2f(m_width, m_height);
}

void UIButton::setText(const std::string &text) {
    m_text = text;
}

std::string UIButton::getText() const {
    return m_text;
}

void UIButton::onMouseMove(const sf::RenderWindow &window) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    m_hovered = m_buttonRect.getGlobalBounds().contains(mousePosition);

    m_buttonRect.setFillColor(m_hovered ? sf::Color(0x4c, 0x58, 0x66, 0x80) : sf::Color(0x80, 0x80, 0x80, 0x80));
}

void UIButton::onMouseClick(const sf::RenderWindow &window) {
    m_clicked = m_hovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (m_clicked)
        m_buttonActionPerformer(window);
}

void UIButton::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;

    m_buttonRect.setPosition(pos);

    std::cout << "Button position changed!" << std::endl;
}

sf::Vector2f UIButton::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UIButton::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    target.draw(m_buttonRect, state);

    const float dx = m_text.length() * uiManager.getSymbolBounds().width / 2;
    const float dy = 4.0f;

    const float x0 = m_x - dx + m_width / 2;
    const float y0 = m_y - dy + m_height / 2;

    for (std::size_t i = 0; i < m_text.length(); ++i) {
        char symb = m_text[i];

        sf::Sprite charSprite = uiManager.getSpriteByCharacter(symb);
        charSprite.setPosition(sf::Vector2f(x0 + i * uiManager.getSymbolBounds().width, y0));
        target.draw(charSprite, state);
    }
}
