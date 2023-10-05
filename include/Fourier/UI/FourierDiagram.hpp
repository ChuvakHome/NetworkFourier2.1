
#ifndef _UI_FOURIER_DIAGRAM_HPP_
#define _UI_FOURIER_DIAGRAM_HPP_

#include <vector>

#include <Fourier/Model/Diagram.hpp>
#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

class UIFourierDiagram : public UIBase {
public:
    UIFourierDiagram(float width = 0.0f, float height = 0.0f);
    void setSize(const sf::Vector2f &size);
    sf::Vector2f getSize() const;
    void setXDrawMode(std::size_t xPoints, std::size_t xPointInGroup);
    void setYDrawMode(std::size_t yPoints, std::size_t yPointInGroup);
    void setShowDiagram(bool flag);
    void setDrawSamplers(bool flag);
    void addSamplerX(float x);
    void clearSamplers();
    void setDesync(float desync);
    void setMinY(float minY);
    void setMaxY(float maxY);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void setDiagram(fourier_diagram diagram);
    void draw(sf::RenderTarget &target, sf::RenderStates state) const;
private:
    float m_x, m_y;
    float m_width, m_height;
    bool m_showDiagram, m_showSamplers;
    std::size_t m_xPoints, m_yPoints;
    std::size_t m_xGroupPointCount, m_yGroupPointCount;
    diagram_point m_diagramPoints[MAX_FOURIER_DIAGRAM_POINTS_COUNT];
    std::size_t m_diagramPointsCount;
    float m_minY, m_maxY;
    float m_desync;
    std::vector<float> m_samplerXPoints;

    void drawXPoints(sf::RenderTarget &target, sf::RenderStates state) const;
    void drawYPoints(sf::RenderTarget &target, sf::RenderStates state) const;
    void drawDiagram(sf::RenderTarget &target, sf::RenderStates state) const;
    void drawSamplers(sf::RenderTarget &target, sf::RenderStates state) const;
};

#endif
