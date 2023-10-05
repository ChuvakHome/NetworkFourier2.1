
#ifndef _UI_CHECKBOX_HPP_
#define _UI_CHECKBOX_HPP_

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

class UICheckbox : public UIBase {
public:
    UICheckbox(bool selected = false, const std::string &tip = "");
    void setTextOffset(float offset);
    void setSelected(bool flag);
    bool isSelected(void) const;
    sf::FloatRect getHoverRect() const;
    void onMouseMove(const sf::RenderWindow &window);
    void onMouseClick(const sf::RenderWindow &window);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void draw(sf::RenderTarget &target, sf::RenderStates state) const;
private:
    float m_x, m_y;
    float m_textOffset;
    bool m_selected;
    bool m_hovered;
    std::string m_tip;
};

#endif
