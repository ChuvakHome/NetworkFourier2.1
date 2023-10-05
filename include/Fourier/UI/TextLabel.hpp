
#ifndef _UI_TEXT_LABEL_HPP_
#define _UI_TEXT_LABEL_HPP_

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

class UITextLabel : public UIBase {
public:
    UITextLabel(const std::string &text = "");
    void setText(const std::string &text);
    std::string getText() const;
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void draw(sf::RenderTarget &target, sf::RenderStates state) const;
private:
    float m_x, m_y;
    std::string m_text;
};

#endif
