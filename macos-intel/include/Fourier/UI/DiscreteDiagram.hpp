
#ifndef _UI_DISCRETE_DIAGRAM_HPP_
#define _UI_DISCRETE_DIAGRAM_HPP_

#include <Fourier/Model/Diagram.hpp>
#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

class UIDiscreteDiagram : public UIBase {
public:
    UIDiscreteDiagram(float width = 0.0f, float height = 0.0f);
    void setSize(const sf::Vector2f &size);
    sf::Vector2f getSize() const;
    void setDrawMode(std::size_t points, std::size_t pointInGroup);
    void setShowDiagram(bool flag);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void setDiagram(discrete_diagram diagram);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    float m_x, m_y;
    float m_width, m_height;
    bool m_showDiagram;
    std::size_t m_points;
    std::size_t m_groupPointCount;
    diagram_point m_diagramPoints[MAX_DIAGRAM_POINTS_COUNT];
    std::size_t m_diagramPointsCount;

    void drawXPoints(sf::RenderTarget &target, sf::RenderStates states) const;
    void drawDiagram(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
