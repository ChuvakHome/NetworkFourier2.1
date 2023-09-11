
#ifndef _UI_PANEL_HPP_
#define _UI_PANEL_HPP_

#include <vector>

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>

struct UIElementInfo {
    UIBase &uiBase;
    float posX;
    float posY;
};

class UIPanel : public UIBase {
public:
    UIPanel(float width = 0.0f, float height = 0.0f, sf::Color color = sf::Color(0x60, 0x60, 0x60, 0x80));
    float getWidth() const;
    float getHeight() const;
    void setSize(const sf::Vector2f &size);
    sf::Vector2f getSize() const;
    void setBackgroundColor(sf::Color color);
    void addUIElement(UIBase &uiBase, const sf::Vector2f &pos);
    void setPositionWithChildren(const sf::Vector2f &pos);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    std::vector<struct UIElementInfo> m_elements;
    sf::RectangleShape m_panelRect;
    float m_x, m_y;
    float m_width, m_height;
};

#endif
