
#ifndef _UI_BUTTON_HPP_
#define _UI_BUTTON_HPP_

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

using buttonActionPerformer = void (*)(const sf::RenderWindow &window);

class UIButton : public UIBase {
public:
    UIButton(const std::string &text = "", buttonActionPerformer performer = nullptr);
    float getWidth() const;
    float getHeight() const;
    void setSize(const sf::Vector2f &size);
    sf::Vector2f getSize() const;
    void setText(const std::string &text);
    std::string getText() const;
    void onMouseMove(const sf::RenderWindow &window);
    void onMouseClick(const sf::RenderWindow &window);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    std::string m_text;
    sf::RectangleShape m_buttonRect;
    float m_x, m_y;
    float m_width, m_height;
    bool m_hovered, m_clicked;
    buttonActionPerformer m_buttonActionPerformer;
};

#endif
