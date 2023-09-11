
#include <Fourier/Global.hpp>
#include <Fourier/UI/TextLabel.hpp>

UITextLabel::UITextLabel(const std::string &text) : m_x(0), m_y(0), m_text(text) {}

void UITextLabel::setText(const std::string &text) {
    m_text = text;
}

std::string UITextLabel::getText() const {
    return m_text;
}

void UITextLabel::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;
}

sf::Vector2f UITextLabel::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UITextLabel::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    sf::FloatRect charBounds = uiManager.getSymbolBounds();

    float x = m_x;
    float y = m_y;

    for (std::size_t i = 0; i < m_text.length(); ++i) {
        if (m_text[i] == '\n') {
            y += charBounds.height;
            continue;
        }
        else if (m_text[i] == '\r') {
            x = m_x;
            continue;
        }

        sf::Sprite charSprite = uiManager.getSpriteByCharacter(m_text[i]);
        charSprite.setPosition(sf::Vector2f(x, y));
        target.draw(charSprite, state);

        x += charSprite.getGlobalBounds().width;
    }
}
