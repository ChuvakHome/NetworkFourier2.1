
#include <Fourier/UI/RadioButton.hpp>

#include <Fourier/Global.hpp>

/* UIRadioButton class definition */
UIRadioButton::UIRadioButton(const std::string &text, bool selected) : m_x(0), m_y(0), m_offset(uiManager.getSymbolBounds().width), m_hovered(false), m_selected(selected), m_text(text) {
    m_radioButtonSprite = uiManager.getSpriteByElementType(selected ? UIElement::RadioButtonActive : UIElement::RadioButtonInactive);
}

void UIRadioButton::setHovered(bool flag) {
    m_hovered = flag;
}

bool UIRadioButton::isHovered() const {
    return m_hovered;
}

void UIRadioButton::setSelected(bool flag) {
    m_selected = flag;
    m_radioButtonSprite = uiManager.getSpriteByElementType(m_selected ? UIElement::RadioButtonActive : UIElement::RadioButtonInactive);
    m_radioButtonSprite.setPosition(getPosition());
}

bool UIRadioButton::isSelected() const {
    return m_selected;
}

sf::FloatRect UIRadioButton::getHoverRect() const {
    return sf::FloatRect(m_x - 3, m_y - 1, 17 * uiManager.getSymbolBounds().width, uiManager.getSymbolBounds().height + 2);
}

void UIRadioButton::setTextOffset(float offset) {
    m_offset = offset;
}

void UIRadioButton::onMouseMove(const sf::RenderWindow &window) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    m_hovered = getHoverRect().contains(mousePosition);
}

void UIRadioButton::onMouseClick(const sf::RenderWindow &window) {
    static_cast<void>(window);

    bool clicked = m_hovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (clicked)
        setSelected(true);
}

void UIRadioButton::setPosition(const sf::Vector2f & pos) {
    m_x = pos.x;
    m_y = pos.y;

    m_radioButtonSprite.setPosition(pos);
}

sf::Vector2f UIRadioButton::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UIRadioButton::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    if (m_hovered) {
        sf::RectangleShape hoverRect(sf::Vector2f(17 * uiManager.getSymbolBounds().width, uiManager.getSymbolBounds().height + 2));
        hoverRect.setPosition(m_x - 3, m_y - 1);
        hoverRect.setFillColor(sf::Color(0x4c, 0x58, 0x66, 0x80));
        hoverRect.setOutlineThickness(1);
        hoverRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
        target.draw(hoverRect, state);
    }

    target.draw(m_radioButtonSprite, state);

    float dx = m_x + m_radioButtonSprite.getLocalBounds().width + m_offset;

    for (std::size_t i = 0; i < m_text.length(); ++i) {
        sf::Sprite charSprite = uiManager.getSpriteByCharacter(m_text[i]);
        charSprite.setPosition(sf::Vector2f(dx + i * charSprite.getLocalBounds().width, m_y));
        target.draw(charSprite, state);
    }
}


/* UIRadioGroup class definition */
UIRadioGroup::UIRadioGroup(radioButtonActionPerformer performer) : m_x(0), m_y(0), m_activeOption(0), m_actionPerformer(performer) {}

void UIRadioGroup::addRadioButton(UIRadioButton &radioButton, const sf::Vector2f &vec) {
    m_radioButtons.push_back(std::make_pair(&radioButton, vec));
}

int UIRadioGroup::getSelectedOption() const {
    return m_activeOption;
}

void UIRadioGroup::onMouseMove(const sf::RenderWindow &window) {
    for (auto &pair: m_radioButtons)
        pair.first->onMouseMove(window);
}

void UIRadioGroup::onMouseClick(const sf::RenderWindow &window) {
    for (auto &pair: m_radioButtons)
        pair.first->onMouseClick(window);

    int newActiveOption = -1;

    for (std::size_t i = 0; i < m_radioButtons.size(); ++i) {
        if (m_radioButtons[i].first->isSelected() && static_cast<int>(i) != m_activeOption) {
            newActiveOption = static_cast<int>(i);
            break;
        }
    }

    if (newActiveOption >= 0 && newActiveOption != m_activeOption) {
        int oldActiveOption = m_activeOption;
        m_activeOption = newActiveOption;

        m_radioButtons[oldActiveOption].first->setSelected(false);

        if (m_actionPerformer != nullptr)
            m_actionPerformer(oldActiveOption, newActiveOption);
    }
}

void UIRadioGroup::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;

    for (auto &pair: m_radioButtons)
        pair.first->setPosition(sf::Vector2f(m_x + pair.second.x, m_y + pair.second.y));
}

void UIRadioGroup::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    for (auto &pair: m_radioButtons)
        target.draw(*pair.first, state);
}
