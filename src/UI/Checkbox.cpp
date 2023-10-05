
#include <Fourier/Global.hpp>
#include <Fourier/UI/Checkbox.hpp>

UICheckbox::UICheckbox(bool selected, const std::string &tip) : m_x(0), m_y(0), m_selected(selected), m_hovered(false), m_tip(tip) {}

void UICheckbox::setTextOffset(float offset) {
    m_textOffset = offset;
}

void UICheckbox::setSelected(bool flag) {
    m_selected = flag;
}

bool UICheckbox::isSelected(void) const {
    return m_selected;
}

sf::FloatRect UICheckbox::getHoverRect() const {
    return sf::FloatRect(m_x - 3, m_y - 1, 17 * uiManager.getSymbolBounds().width, 10);
}

void UICheckbox::onMouseMove(const sf::RenderWindow &window) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    m_hovered = getHoverRect().contains(mousePosition);
}

void UICheckbox::onMouseClick(const sf::RenderWindow &window) {
    static_cast<void>(window);

    bool clicked = m_hovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (clicked)
        setSelected(!m_selected);
}

void UICheckbox::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;
}

sf::Vector2f UICheckbox::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UICheckbox::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    if (m_hovered) {
        sf::FloatRect hoverRectBounds = getHoverRect();
        sf::RectangleShape hoverRect(sf::Vector2f(hoverRectBounds.width, hoverRectBounds.height));
        hoverRect.setPosition(sf::Vector2f(hoverRectBounds.left, hoverRectBounds.top));
        hoverRect.setFillColor(sf::Color(0x4c, 0x58, 0x66, 0x80));
        hoverRect.setOutlineThickness(1);
        hoverRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
        target.draw(hoverRect, state);
    }

    sf::Sprite checkboxSprite = m_selected ? uiManager.getSpriteByElementType(UIElement::CheckboxActive) : uiManager.getSpriteByElementType(UIElement::CheckboxInactive);
    checkboxSprite.setPosition(sf::Vector2f(m_x, m_y));
    target.draw(checkboxSprite, state);

    // sf::FloatRect charBounds = uiManager.getSpriteByCharacter('0').getGlobalBounds();
    float spriteOffset = 0.0f;

    for (std::size_t i = 0; i < m_tip.length(); ++i) {
        sf::Sprite charSprite = uiManager.getSpriteByCharacter(m_tip[i]);
        charSprite.setPosition(sf::Vector2f(m_x + checkboxSprite.getGlobalBounds().width + m_textOffset + spriteOffset, m_y));
        target.draw(charSprite, state);
        spriteOffset += charSprite.getGlobalBounds().width;
    }
}
