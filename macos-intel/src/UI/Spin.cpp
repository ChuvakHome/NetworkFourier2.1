
#include <Fourier/UI/Spin.hpp>

#include <cmath>

#include <Fourier/Global.hpp>
#include <Fourier/Utils.hpp>

enum SpinHoverState {
    SpinNoHover = 0,
    SpinLeftStepButtonHovered,
    SpinInputFieldHovered,
    SpinRightStepButtonHovered
};

template<typename T> static bool equals(T x, T y, double precision = 0.00001) {
    return std::fabs(x - y) < precision;
}

template<typename T> static bool not_equals(T x, T y, double precision = 0.00001) {
    return std::fabs(x - y) > precision;
}

template<typename T>UISpin<T>::UISpin(T min, T max, T step, T superStep, const std::string &tip, spinActionPerformer performer) : m_min(min), m_max(max), m_maxSymbolsCount(5), m_x(0), m_y(0), m_value(min), m_step(step), m_superStep(superStep), m_textOffset(2), m_inputFieldSelected(false), m_hoverState(0), m_tip(tip), m_spinActionPerformer(performer) {
    sf::FloatRect charBounds = uiManager.getSpriteByCharacter('0').getLocalBounds();

    std::size_t len = 1;
    T temp = m_max;

    while (temp >= 10) {
        ++len;
        temp /= 10;
    }

    if (len < m_maxSymbolsCount)
        len = m_maxSymbolsCount;

    m_inputRect = sf::RectangleShape(sf::Vector2f(charBounds.width * len + 4, charBounds.height + 2));
    m_inputRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));
    m_inputRect.setOutlineThickness(1);
    m_inputRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
    m_inputRect.setPosition(sf::Vector2f(m_x, m_y));

    m_leftStepButtonRect = sf::RectangleShape(sf::Vector2f(charBounds.width + 3, charBounds.height + 2));
    m_leftStepButtonRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));
    m_leftStepButtonRect.setOutlineThickness(1);
    m_leftStepButtonRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
    m_leftStepButtonRect.setPosition(sf::Vector2f(m_x - m_leftStepButtonRect.getLocalBounds().width + 1, m_y));

    m_rightStepButtonRect = sf::RectangleShape(sf::Vector2f(charBounds.width + 3, charBounds.height + 2));
    m_rightStepButtonRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));
    m_rightStepButtonRect.setOutlineThickness(1);
    m_rightStepButtonRect.setOutlineColor(sf::Color(0xe6, 0xe6, 0xe6));
    m_rightStepButtonRect.setPosition(sf::Vector2f(m_x + m_inputRect.getGlobalBounds().width - 1, m_y));
}

template<typename T> void UISpin<T>::setSize(const sf::Vector2f &size) {
    m_inputRect.setSize(size);
    m_leftStepButtonRect.setSize(size);
    m_rightStepButtonRect.setSize(size);

}

template<typename T> sf::Vector2f UISpin<T>::getSize() const {
    return m_inputRect.getSize();
}

template<typename T> void UISpin<T>::setMaxSymbolsCount(std::size_t count) {
    if (count > 0)
        m_maxSymbolsCount = count;
}

template<typename T> void UISpin<T>::increaseValue(bool superIncrease) {
    setValue(m_value + (superIncrease ? m_superStep : m_step));
}

template<typename T> void UISpin<T>::decreaseValue(bool superDecrease) {
    setValue(m_value - (superDecrease ? m_superStep : m_step));
}

template<typename T> void UISpin<T>::setStep(T step) {
    m_step = step;
}

template<typename T> void UISpin<T>::setSuperStep(T superStep) {
    m_superStep = superStep;
}

template<typename T> void UISpin<T>::setValue(T value) {
    if (value < m_min)
        m_value = m_min;
    else if (value > m_max)
        m_value = m_max;
    else
        m_value = value;
}

template<typename T> T UISpin<T>::getValue(void) const {
    return m_value;
}

template<typename T> void UISpin<T>::setTextOffset(float offset) {
    m_textOffset = offset;
}

template<typename T> void UISpin<T>::onMouseMove(const sf::RenderWindow &window) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    sf::RectangleShape rectangles[] = {
        m_leftStepButtonRect,
        m_inputRect,
        m_rightStepButtonRect
    };

    sf::Color hoverColor = sf::Color(0x4c, 0x58, 0x66, 0x80);
    sf::Color unhoverColor = sf::Color(0x80, 0x80, 0x80, 0x80);

    m_hoverState = 0;

    int rect = 0;

    while (rect < 3 && m_hoverState == 0) {
        if (rectangles[rect].getGlobalBounds().contains(mousePosition))
            m_hoverState = rect + 1;

        ++rect;
    }

    SpinHoverState hoverState = static_cast<SpinHoverState>(m_hoverState);

    switch (hoverState) {
        case SpinLeftStepButtonHovered:
            m_leftStepButtonRect.setFillColor(hoverColor);
            m_rightStepButtonRect.setFillColor(unhoverColor);
            break;
        case SpinRightStepButtonHovered:
            m_leftStepButtonRect.setFillColor(unhoverColor);
            m_rightStepButtonRect.setFillColor(hoverColor);
            break;
        default:
            m_leftStepButtonRect.setFillColor(unhoverColor);
            m_rightStepButtonRect.setFillColor(unhoverColor);
            break;
    }
}

template<typename T> void UISpin<T>::onMouseClick(const sf::RenderWindow &window) {
    SpinHoverState hoverState = static_cast<SpinHoverState>(m_hoverState);

    m_inputFieldSelected = false;
    m_inputRect.setFillColor(sf::Color(0x80, 0x80, 0x80, 0x80));

    T oldval = m_value;

    switch (hoverState) {
        case SpinLeftStepButtonHovered:
            decreaseValue(is_shift_key_pressed());
            break;
        case SpinInputFieldHovered:
            m_inputFieldSelected = true;
            m_inputRect.setFillColor(sf::Color(0x60, 0x60, 0x60, 0x60));
            break;
        case SpinRightStepButtonHovered:
            increaseValue(is_shift_key_pressed());
            break;
        default:
            break;
    }

    if (not_equals(m_value, oldval) && m_spinActionPerformer != nullptr)
        m_spinActionPerformer({ oldval, m_value }, window);
}

template<typename T> void UISpin<T>::onKeyPressed(const sf::RenderWindow &window) {
    bool leftArrowPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool rightArrowPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    T oldval = m_value;

    if (leftArrowPressed != rightArrowPressed && m_inputFieldSelected) {
        if (leftArrowPressed)
            decreaseValue(is_shift_key_pressed());
        else
            increaseValue(is_shift_key_pressed());

        if (not_equals(m_value, oldval) && m_spinActionPerformer != nullptr)
            m_spinActionPerformer({ oldval, m_value }, window);
    }
}

template<typename T> void UISpin<T>::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;

    m_inputRect.setPosition(sf::Vector2f(m_x, m_y));
    m_leftStepButtonRect.setPosition(sf::Vector2f(m_x - m_leftStepButtonRect.getLocalBounds().width + 1, m_y));
    m_rightStepButtonRect.setPosition(sf::Vector2f(m_x + m_inputRect.getGlobalBounds().width - 1, m_y));
}

template<typename T> sf::Vector2f UISpin<T>::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

template<typename T> void UISpin<T>::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    std::size_t len = std::to_string(m_max).length();

    if (len > m_maxSymbolsCount)
        len = m_maxSymbolsCount;

    sf::FloatRect charBounds = uiManager.getSymbolBounds();

    target.draw(m_inputRect, state);
    target.draw(m_leftStepButtonRect, state);
    target.draw(m_rightStepButtonRect, state);

    sf::Sprite leftStepButtonSprite = uiManager.getSpriteByElementType(UIElement::ArrowLeft);
    leftStepButtonSprite.setPosition(m_x - leftStepButtonSprite.getLocalBounds().width - 2, m_y + 1);
    target.draw(leftStepButtonSprite, state);

    sf::Sprite rightStepButtonSprite = uiManager.getSpriteByElementType(UIElement::ArrowRight);
    rightStepButtonSprite.setPosition(sf::Vector2f(m_inputRect.getGlobalBounds().left + m_inputRect.getGlobalBounds().width + 2, m_y + 1));
    target.draw(rightStepButtonSprite, state);

    const std::size_t tiplen = m_tip.length();
    float dx = m_leftStepButtonRect.getGlobalBounds().left - m_textOffset;

    for (std::size_t i = 0; i < tiplen; ++i) {
        sf::Sprite charSprite = uiManager.getSpriteByCharacter(m_tip[i]);
        charSprite.setPosition(sf::Vector2f(dx - (tiplen - i) * charBounds.width, m_y));
        target.draw(charSprite, state);
    }

    std::string valstr = std::to_string(m_value);

    if (valstr.length() > m_maxSymbolsCount)
        valstr = valstr.erase(m_maxSymbolsCount, valstr.length() - m_maxSymbolsCount);

    len = valstr.length();

    std::size_t len2 = 0;
    float x0 = m_x + m_inputRect.getLocalBounds().width - (3 + charBounds.width + 2);

    while (len2 < len) {
        sf::Sprite charSprite = uiManager.getSpriteByCharacter(valstr[len - len2 - 1]);
        charSprite.setPosition(sf::Vector2f(x0 - charBounds.width * len2, m_y + 1));
        target.draw(charSprite, state);

        ++len2;
    }
}
