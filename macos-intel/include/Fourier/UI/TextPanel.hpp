
#ifndef _UI_TEXT_PANEL_HPP_
#define _UI_TEXT_PANEL_HPP_

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

class UITextPanel : public UIBase {
public:
    UITextPanel(const std::string &text = "", float width = 0, float height = 0);
    void setSize(const sf::Vector2f &size);
    sf::Vector2f getSize() const;
    void onMouseMove(const sf::RenderWindow &window);
    void onMouseClick(const sf::RenderWindow &window);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    float m_x, m_y;
    float m_width, m_height;
    bool m_hovered;
    sf::RectangleShape m_textPanelRect;
    std::string m_text;
};

#endif
