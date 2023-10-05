
#include <Fourier/Global.hpp>

#include <Fourier/UI/Dropdown.hpp>

enum DropdownHoverState {
    DropdownNoHover = 0,
    DropdownTextFieldHovered,
    DropdownButtonHovered,
    DropdownOptionHovered
};

// template<typename T> UIDropdown<T>::UIDropdown() : m_x(0), m_y(0), m_maxSymbols(5), m_listOption(0), m_activeOption(0), m_hoverState(0), m_optionListVisible(false), m_dropdownActionPerformer(nullptr) {}

template<typename T> UIDropdown<T>::UIDropdown(dropdownActionPerformer actionPerformer, std::size_t maxSymbols) : m_x(0), m_y(0), m_maxSymbols(maxSymbols), m_listOption(0), m_activeOption(0), m_hoverState(0), m_optionListVisible(false), m_dropdownActionPerformer(actionPerformer) {
    m_textRect = sf::RectangleShape(sf::Vector2f(uiManager.getSymbolBounds().width * maxSymbols + 4, uiManager.getSymbolBounds().height + 2));
    m_textRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0xff));
    m_textRect.setOutlineThickness(1);
    m_textRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
    m_textRect.setPosition(sf::Vector2f(m_x, m_y));

    m_buttonRect = sf::RectangleShape(sf::Vector2f(uiManager.getSymbolBounds().width + 3, uiManager.getSymbolBounds().height + 2));
    m_buttonRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0xff));
    m_buttonRect.setOutlineThickness(1);
    m_buttonRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
    m_buttonRect.setPosition(sf::Vector2f(m_x + m_textRect.getGlobalBounds().width - 1, m_y));
}

template<typename T> void UIDropdown<T>::setSize(const sf::Vector2f &size) {
    m_textRect.setSize(size);
    m_buttonRect.setSize(size);
}

template<typename T> sf::Vector2f UIDropdown<T>::getSize() const {
    return m_textRect.getSize();
}

template<typename T> void UIDropdown<T>::setMaxSymbolsCount(std::size_t count) {
    if (count > 0)
        m_maxSymbols = count;
}

template<typename T> void UIDropdown<T>::addOption(const std::string &text, const T &option) {
    m_options.push_back(std::make_pair(text, option));
}

template<typename T> void UIDropdown<T>::selectOption(int option) {
    if (option >= 0 && option < m_options.size())
        m_activeOption = option;
}

template<typename T> T UIDropdown<T>::getSelectedOption(void) const {
    return m_options[m_activeOption].second;
}

template<typename T> void UIDropdown<T>::onKeyPressed(sf::RenderWindow &window) {
    if (m_optionListVisible) {
        bool upArrowPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
        bool downArrowPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        bool enterPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

        if (enterPressed) {
            m_optionListVisible = false;

            if (m_activeOption != m_listOption && m_dropdownActionPerformer != nullptr) {
                T oldval = m_options[m_activeOption].second;
                T newval = m_options[m_listOption].second;

                m_activeOption = m_listOption;
                m_dropdownActionPerformer({ oldval, newval }, window);
            }
        }
        else if (upArrowPressed != downArrowPressed) {
            if (m_listOption > 0 && upArrowPressed)
                m_listOption--;
            else if (m_listOption < m_options.size() - 1 && downArrowPressed)
                m_listOption++;
        }
    }
}

template<typename T> void UIDropdown<T>::onMouseMove(sf::RenderWindow &window) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    sf::RectangleShape rectangles[] = {
        m_textRect,
        m_buttonRect
    };

    sf::Color hoverColor = sf::Color(0x4c, 0x58, 0x66, 0xff);
    sf::Color unhoverColor = sf::Color(0x80, 0x80, 0x80, 0xff);

    m_hoverState = DropdownHoverState::DropdownNoHover;

    int rect = 0;

    while (rect < 2 && m_hoverState == DropdownHoverState::DropdownNoHover) {
        if (rectangles[rect].getGlobalBounds().contains(mousePosition))
            m_hoverState = rect + 1;

        ++rect;
    }

    DropdownHoverState hoverState = static_cast<DropdownHoverState>(m_hoverState);

    switch (hoverState) {
        case DropdownTextFieldHovered:
            m_textRect.setFillColor(hoverColor);
            m_buttonRect.setFillColor(unhoverColor);
            break;
        case DropdownButtonHovered:
            m_textRect.setFillColor(unhoverColor);
            m_buttonRect.setFillColor(hoverColor);
            break;
        default:
            m_textRect.setFillColor(unhoverColor);
            m_buttonRect.setFillColor(unhoverColor);
            break;
    }

    if (m_hoverState == DropdownHoverState::DropdownNoHover && m_optionListVisible) {
        sf::FloatRect textRectBounds = m_textRect.getGlobalBounds();

        for (std::size_t i = 0; i < m_options.size(); ++i) {
            sf::RectangleShape rect(sf::Vector2f(textRectBounds.width - 2, textRectBounds.height - 3));
            rect.setPosition(sf::Vector2f(textRectBounds.left + 1, m_y + (i + 1) * textRectBounds.height));

            if (rect.getGlobalBounds().contains(mousePosition)) {
                m_hoverState = DropdownHoverState::DropdownOptionHovered;
                m_listOption = i;
            }
        }

        // if (m_hoverState == 0) {
        //     m_optionListVisible = false;
        //     m_listOption = m_activeOption;
        // }
    }
}

template<typename T> void UIDropdown<T>::onMouseClick(sf::RenderWindow &window) {
    bool selected = m_hoverState != 0 && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    DropdownHoverState hoverState = static_cast<DropdownHoverState>(m_hoverState);

    switch (hoverState) {
        case DropdownTextFieldHovered:
        case DropdownButtonHovered:
            if (selected) {
                m_optionListVisible = !m_optionListVisible;
                m_listOption = m_activeOption;
            }
            break;
        case DropdownOptionHovered:
            if (selected && m_optionListVisible) {
                m_optionListVisible = false;

                if (m_listOption != m_activeOption && m_dropdownActionPerformer != nullptr) {
                    T oldval = m_options[m_activeOption].second;
                    T newval = m_options[m_listOption].second;

                    m_activeOption = m_listOption;
                    m_dropdownActionPerformer({ oldval, newval }, window);
                }

                selected = false;
            }
            break;
        default:
            selected = false;
            m_optionListVisible = false;
            break;
    }
}

template<typename T> void UIDropdown<T>::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;

    m_textRect.setPosition(sf::Vector2f(m_x, m_y));
    m_buttonRect.setPosition(sf::Vector2f(m_x + m_textRect.getGlobalBounds().width - 1, m_y));
}

template<typename T> sf::Vector2f UIDropdown<T>::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

template<typename T> void UIDropdown<T>::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    target.draw(m_textRect, state);
    target.draw(m_buttonRect, state);

    sf::Sprite buttonSprite = uiManager.getSpriteByElementType(m_optionListVisible ? UIElement::ArrowUp : UIElement::ArrowDown);
    buttonSprite.setPosition(sf::Vector2f(m_buttonRect.getGlobalBounds().left + 3, m_y + 1));
    target.draw(buttonSprite, state);

    const std::string &text = m_options[m_activeOption].first;
    float dx = m_x + (m_maxSymbols - text.length()) * uiManager.getSymbolBounds().width / 2 + 2;

    for (std::size_t i = 0; i < text.length(); ++i) {
        sf::Sprite charSprite = uiManager.getSpriteByCharacter(text[i]);
        charSprite.setPosition(sf::Vector2f(dx + i * uiManager.getSymbolBounds().width, m_y + 1));
        target.draw(charSprite, state);
    }

    if (m_optionListVisible) {
        sf::FloatRect textRectBounds = m_textRect.getGlobalBounds();
        sf::RectangleShape optionListRectangle(sf::Vector2f(textRectBounds.width - 2, textRectBounds.height * m_options.size()));
        // optionListRectangle.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));
        // optionListRectangle.setFillColor(sf::Color(0x80, 0x80, 0x80, 0xcf));
        optionListRectangle.setFillColor(sf::Color(0x80, 0x80, 0x80, 0xff));
        optionListRectangle.setOutlineThickness(1);
        optionListRectangle.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
        optionListRectangle.setPosition(sf::Vector2f(m_x, m_y + textRectBounds.height - 1));
        target.draw(optionListRectangle, state);

        sf::RectangleShape activeOptionRectangle(sf::Vector2f(textRectBounds.width - 2, textRectBounds.height - 3));
        activeOptionRectangle.setFillColor(sf::Color(0x4c, 0x58, 0x66, 0x80));
        activeOptionRectangle.setOutlineThickness(1);
        activeOptionRectangle.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
        activeOptionRectangle.setPosition(sf::Vector2f(textRectBounds.left + 1, m_y + (m_listOption + 1) * textRectBounds.height));
        target.draw(activeOptionRectangle, state);

        for (std::size_t i = 0; i < m_options.size(); ++i) {
            const std::string &text = m_options[i].first;
            float dx = m_x + (m_maxSymbols - text.length()) * uiManager.getSymbolBounds().width / 2 + 2;

            for (std::size_t j = 0; j < text.length(); ++j) {
                sf::Sprite charSprite = uiManager.getSpriteByCharacter(text[j]);
                charSprite.setPosition(sf::Vector2f(dx + j * uiManager.getSymbolBounds().width, m_y + (i + 1) * textRectBounds.height));
                target.draw(charSprite, state);
            }
        }
    }
}
