
#include <Fourier/Global.hpp>
#include <Fourier/UI/TextField.hpp>

#define recompute_cursor_position \
    do { \
        m_fieldCursor.setPosition(sf::Vector2f(m_x + m_text.length() * uiManager.getSymbolBounds().width + 2, m_y + 1)); \
    } while(0)

UITextField::UITextField(std::size_t maxSymbolsCount, bool focused) : m_maxSymbols(maxSymbolsCount), m_x(0), m_y(0), m_focused(focused) {
    m_fieldRect = sf::RectangleShape(sf::Vector2f(17 * uiManager.getSymbolBounds().width, 11));
    m_fieldRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));
    m_fieldRect.setOutlineThickness(1);
    m_fieldRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
}

void UITextField::setText(const std::string &text) {
    m_text = text;
    recompute_cursor_position;
}

std::string UITextField::getText() const {
    return m_text;
}

void UITextField::addSymbol(char symb) {
    if (m_focused && m_text.length() < m_maxSymbols) {
        m_text += symb;
        recompute_cursor_position;
    }
}

void UITextField::removeSymbol() {
    if (m_focused && !m_text.empty()) {
        m_text.pop_back();
        recompute_cursor_position;
    }
}

void UITextField::clearText() {
    setText("");
}

void UITextField::setFocused(bool focused) {
    m_focused = focused;
}

void UITextField::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;

    m_fieldRect.setPosition(pos);
    recompute_cursor_position;
}

sf::Vector2f UITextField::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UITextField::setCursorDelayVisible(const sf::Time &delayVisible) {
    m_fieldCursor.setDelayVisible(delayVisible);
}

void UITextField::setCursorDelayInvisible(const sf::Time &delayInvisible) {
    m_fieldCursor.setDelayInvisible(delayInvisible);
}

void UITextField::setCursorDelay(const sf::Time &delay) {
    m_fieldCursor.setDelay(delay);
}

void UITextField::update(const sf::RenderWindow &window) {
    m_fieldCursor.update(window);
}

void UITextField::onMouseClick(const sf::RenderWindow &window) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    m_focused = m_fieldRect.getGlobalBounds().contains(mousePosition) && sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

void UITextField::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    target.draw(m_fieldRect, state);

    const float x0 = m_x;
    const float y0 = m_y;

    for (std::size_t i = 0; i < m_text.length(); ++i) {
        sf::Sprite charSprite = uiManager.getSpriteByCharacter(m_text[i]);
        charSprite.setPosition(x0 + i * charSprite.getLocalBounds().width + 2, y0 + 1);
        target.draw(charSprite, state);
    }

    if (m_focused)
        target.draw(m_fieldCursor, state);
}
