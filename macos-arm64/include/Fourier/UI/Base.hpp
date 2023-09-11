
#ifndef _UI_BASE_HPP_
#define _UI_BASE_HPP_

#include <SFML/Graphics.hpp>

class UIBase : public sf::Drawable, sf::Transformable {
public:
    virtual void setSize(const sf::Vector2f &size) { static_cast<void>(size); }
    virtual sf::Vector2f getSize() const { return sf::Vector2f(0.0f, 0.0f); }
    virtual void setPosition(const sf::Vector2f &pos) = 0;
    virtual sf::Vector2f getPosition() const { return sf::Vector2f(0.0f, 0.0f); }
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const = 0;
};

#endif
