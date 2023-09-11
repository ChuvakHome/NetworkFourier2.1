
#ifndef _UI_SPIN_HPP_
#define _UI_SPIN_HPP_

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

template<typename T>
class UISpin : public UIBase {
public:
    struct spin_change {
        T oldval;
        T newval;
    };

    using spinActionPerformer = void (*)(const spin_change change, const sf::RenderWindow &window);

    UISpin(T min = 0, T max = 255, T step = 1, T superStep = 1, const std::string &tip = "", spinActionPerformer performer = nullptr);
    void setSize(const sf::Vector2f &size);
    sf::Vector2f getSize() const;
    void setMaxSymbolsCount(std::size_t count);
    void increaseValue(bool superIncrease = false);
    void decreaseValue(bool superDecrease = false);
    void setStep(T step);
    void setSuperStep(T superStep);
    void setValue(T value);
    T getValue(void) const;
    void setTextOffset(float offset);
    void onMouseMove(const sf::RenderWindow &window);
    void onMouseClick(const sf::RenderWindow &window);
    void onKeyPressed(const sf::RenderWindow &window);
    // void onKeyReleased(const sf::RenderWindow &window);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    T m_min, m_max;
    std::size_t m_maxSymbolsCount;
    float m_x, m_y;
    T m_value;
    T m_step, m_superStep;
    float m_textOffset;
    sf::RectangleShape m_leftStepButtonRect;
    sf::RectangleShape m_rightStepButtonRect;
    sf::RectangleShape m_inputRect;
    bool m_inputFieldSelected;
    int m_hoverState;
    std::string m_tip;
    spinActionPerformer m_spinActionPerformer;
};

#endif
