
#include <Fourier/UI/FourierDiagram.hpp>

#include <cmath>
#include <cstdlib>

static int diagram_point_comparator(const void *x, const void *y) {
    const diagram_point *p1 = static_cast<const diagram_point *>(x);
    const diagram_point *p2 = static_cast<const diagram_point *>(y);

    return p1->x > p2->x ? 1 : (fabs(p1->x - p2->x) <= 0.00001 ? 0 : -1);
}

UIFourierDiagram::UIFourierDiagram(float width, float height) : m_x(0), m_y(0), m_width(width), m_height(height), m_showDiagram(true), m_showSamplers(true), m_xPoints(8), m_yPoints(20), m_xGroupPointCount(8), m_yGroupPointCount(5), m_diagramPointsCount(MAX_DIAGRAM_POINTS_COUNT), m_minY(-height * 0.5f), m_maxY(height * 0.5f), m_desync(0.0f) {
    for (std::size_t i = 0; i < m_diagramPointsCount; ++i)
        m_diagramPoints[i] = { static_cast<float>(i), -1 };
}

void UIFourierDiagram::setSize(const sf::Vector2f &size) {
    m_width = size.x;
    m_height = size.y;
}

sf::Vector2f UIFourierDiagram::getSize() const {
    return sf::Vector2f(m_width, m_height);
}

void UIFourierDiagram::setXDrawMode(std::size_t xPoints, std::size_t xPointInGroup) {
    m_xPoints = xPoints;
    m_xGroupPointCount = xPointInGroup;
}

void UIFourierDiagram::setYDrawMode(std::size_t yPoints, std::size_t yPointInGroup) {
    m_yPoints = yPoints;
    m_yGroupPointCount = yPointInGroup;
}

void UIFourierDiagram::setShowDiagram(bool flag) {
    m_showDiagram = flag;
}

void UIFourierDiagram::setDrawSamplers(bool flag) {
    m_showSamplers = flag;
}

void UIFourierDiagram::addSamplerX(float x) {
    m_samplerXPoints.push_back(x);
}

void UIFourierDiagram::clearSamplers() {
    m_samplerXPoints.clear();
}

void UIFourierDiagram::setDesync(float desync) {
    m_desync = desync;
}

void UIFourierDiagram::setMinY(float minY) {
    m_minY = minY;
}

void UIFourierDiagram::setMaxY(float maxY) {
    m_maxY = maxY;
}

void UIFourierDiagram::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;
}

sf::Vector2f UIFourierDiagram::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UIFourierDiagram::setDiagram(fourier_diagram diagram) {
    if (diagram.diagram_points_count > 0) {
        m_diagramPointsCount = diagram.diagram_points_count <= MAX_FOURIER_DIAGRAM_POINTS_COUNT ? diagram.diagram_points_count : MAX_FOURIER_DIAGRAM_POINTS_COUNT;

        for (std::size_t i = 0; i < m_diagramPointsCount; ++i)
            m_diagramPoints[i] = diagram.diagram_points[i];

        qsort(m_diagramPoints, m_diagramPointsCount, sizeof(diagram_point), diagram_point_comparator);
    }
}

void UIFourierDiagram::drawXPoints(sf::RenderTarget &target, sf::RenderStates state) const {
    std::size_t i = 0;
    float x = 0;
    float dx = m_width / m_xPoints;
    float yOffset = m_height / 2;
    sf::Color vertexColor(0xff, 0xff, 0xff, 0xff);

    sf::Vertex vertices2[] = {
        sf::Vertex(sf::Vector2f(m_x + m_width, m_y + yOffset - 7), vertexColor),
        sf::Vertex(sf::Vector2f(m_x + m_width, m_y + yOffset + 8), vertexColor),
    };

    target.draw(vertices2, 2, sf::PrimitiveType::Lines, state);

    while (x < m_width) {
        ++i;

        if (i == m_xGroupPointCount) {
            vertices2[0] = sf::Vertex(sf::Vector2f(m_x + x + dx, m_y + yOffset - 7), vertexColor);
            vertices2[1] = sf::Vertex(sf::Vector2f(m_x + x + dx, m_y + yOffset + 8), vertexColor);

            i = 0;
        }
        else {
            vertices2[0] = sf::Vertex(sf::Vector2f(m_x + x + dx, m_y + yOffset - 2), vertexColor);
            vertices2[1] = sf::Vertex(sf::Vector2f(m_x + x + dx, m_y + yOffset + 3), vertexColor);
        }

        target.draw(vertices2, 2, sf::PrimitiveType::Lines, state);

        x += dx;
    }
}

void UIFourierDiagram::drawYPoints(sf::RenderTarget &target, sf::RenderStates state) const {
    std::size_t i = 0;
    float y = 0;
    float dy = (m_height - 10) / m_yPoints;
    float yOffset = (m_height - 10) / 2;
    sf::Color vertexColor(0xff, 0xff, 0xff, 0xff);

    sf::Vertex vertices2[] = {
        sf::Vertex(sf::Vector2f(m_x - 9, m_y + 5), vertexColor),
        sf::Vertex(sf::Vector2f(m_x + 8, m_y + 5), vertexColor),
    };

    target.draw(vertices2, 2, sf::PrimitiveType::Lines, state);

    vertices2[0] = sf::Vertex(sf::Vector2f(m_x - 9, m_y + m_height - 5), vertexColor);
    vertices2[1] = sf::Vertex(sf::Vector2f(m_x + 8, m_y + m_height - 5), vertexColor);

    target.draw(vertices2, 2, sf::PrimitiveType::Lines, state);

    while (y < yOffset) {
        ++i;

        if (i == m_yGroupPointCount) {
            vertices2[0] = sf::Vertex(sf::Vector2f(m_x - 9, m_y + y + dy + 5), vertexColor);
            vertices2[1] = sf::Vertex(sf::Vector2f(m_x + 8, m_y + y + dy + 5), vertexColor);

            i = 0;
        }
        else {
            vertices2[0] = sf::Vertex(sf::Vector2f(m_x - 6, m_y + y + dy + 5), vertexColor);
            vertices2[1] = sf::Vertex(sf::Vector2f(m_x + 5, m_y + y + dy + 5), vertexColor);
        }

        target.draw(vertices2, 2, sf::PrimitiveType::Lines, state);

        y += dy;
    }

    y = 0;
    i = 0;

    while (y + dy < yOffset) {
        if (i == m_yGroupPointCount) {
            vertices2[0] = sf::Vertex(sf::Vector2f(m_x - 10, m_y + yOffset + y + dy), vertexColor);
            vertices2[1] = sf::Vertex(sf::Vector2f(m_x + 9, m_y + yOffset + y + dy), vertexColor);

            i = 0;
        }
        else {
            vertices2[0] = sf::Vertex(sf::Vector2f(m_x - 5, m_y + yOffset + y + dy), vertexColor);
            vertices2[1] = sf::Vertex(sf::Vector2f(m_x + 4, m_y + yOffset + y + dy), vertexColor);
        }

        target.draw(vertices2, 2, sf::PrimitiveType::Lines, state);

        y += dy;
        ++i;
    }
}

void UIFourierDiagram::drawDiagram(sf::RenderTarget &target, sf::RenderStates state) const {
    sf::Vertex vertices[MAX_FOURIER_DIAGRAM_POINTS_COUNT];
    sf::Vertex vertices2[MAX_FOURIER_DIAGRAM_POINTS_COUNT];

    sf::Color vertexColor(0xff, 0xff, 0xff, 0x66);
    float yOffset = m_height / 2;
    // float x = m_x;
    float dx = m_width / m_xPoints;
    std::size_t j = 0;
    std::size_t visibleDiagramPoints = 0;

    // diagram_point prevPoint;

    for (std::size_t i = 0; i < m_diagramPointsCount; ++i) {
        diagram_point point = m_diagramPoints[i];

        if (point.y >= m_minY && point.y <= m_maxY) {
            vertices[j] = sf::Vertex(sf::Vector2f(m_x + dx * point.x, m_y + yOffset - (yOffset - 5) * point.y * 0.5f), vertexColor);
            vertices2[j] = sf::Vertex(sf::Vector2f(m_x + dx * point.x, m_y + yOffset - (yOffset - 5) * point.y * 0.5f + 1), vertexColor);
            ++j;
            ++visibleDiagramPoints;
        }
    }

    target.draw(vertices, visibleDiagramPoints, sf::LineStrip, state);
    target.draw(vertices2, visibleDiagramPoints, sf::LineStrip, state);
}

void UIFourierDiagram::drawSamplers(sf::RenderTarget &target, sf::RenderStates state) const {
    sf::Vertex vertices1[4];
    sf::Vertex vertices2[4];

    float yOffset = m_height / 2;
    float dx = m_width / m_xPoints;
    float desync2 = m_desync * 0.5f;
    float y0 = m_y + yOffset;

    sf::Color color0(0xff, 0xff, 0xff, 0x0), color1(0xff, 0xff, 0xff, 0x33);

    for (float x: m_samplerXPoints) {
        vertices1[0] = sf::Vertex(sf::Vector2f(m_x + dx * (x + desync2), y0 + yOffset - 5), color0);
        vertices1[1] = sf::Vertex(sf::Vector2f(m_x + dx * (x - 0.02f - desync2), y0 + yOffset - 5), color0);
        vertices1[2] = sf::Vertex(sf::Vector2f(m_x + dx * (x - 0.02f - desync2), y0), color1);
        vertices1[3] = sf::Vertex(sf::Vector2f(m_x + dx * (x + desync2), y0), color1);

        target.draw(vertices1, 4, sf::PrimitiveType::Quads, state);

        vertices2[0] = sf::Vertex(sf::Vector2f(m_x + dx * (x + desync2), y0 - yOffset + 5), color0);
        vertices2[1] = sf::Vertex(sf::Vector2f(m_x + dx * (x - 0.02f - desync2), y0 - yOffset + 5), color0);
        vertices2[2] = sf::Vertex(sf::Vector2f(m_x + dx * (x - 0.02f - desync2), y0), color1);
        vertices2[3] = sf::Vertex(sf::Vector2f(m_x + dx * (x + desync2), y0), color1);

        target.draw(vertices2, 4, sf::PrimitiveType::Quads, state);
    }
}

void UIFourierDiagram::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    if (m_showDiagram)
        drawDiagram(target, state);

    sf::Color vertexColor(0xff, 0xff, 0xff, 0xff);

    sf::Vertex vertices1[] = {
        sf::Vertex(sf::Vector2f(m_x, m_y), vertexColor),
        sf::Vertex(sf::Vector2f(m_x, m_y + m_height), vertexColor),

        sf::Vertex(sf::Vector2f(m_x - 8, m_y + 5), vertexColor),
        sf::Vertex(sf::Vector2f(m_x + 8, m_y + 5), vertexColor),

        sf::Vertex(sf::Vector2f(m_x - 9, m_y + m_height / 2), vertexColor),
        sf::Vertex(sf::Vector2f(m_x + m_width, m_y + m_height / 2), vertexColor),

        sf::Vertex(sf::Vector2f(m_x - 9, m_y + m_height - 5), vertexColor),
        sf::Vertex(sf::Vector2f(m_x + 8, m_y + m_height - 5), vertexColor)
    };

    target.draw(vertices1, sizeof(vertices1) / sizeof(sf::Vertex), sf::PrimitiveType::Lines, state);

    // sf::Vertex vertices[] = {
    //     sf::Vertex(sf::Vector2f(m_x, m_y + m_height / 2 + 0.2 * (m_height - 10) / 4), sf::Color::Red),
    //     sf::Vertex(sf::Vector2f(m_x + m_width, m_y + m_height / 2 + 0.2 * (m_height - 10) / 4), sf::Color::Red),
    // };

    /* BORDERS */
    // for (int i = 1; i <= 5; ++i) {
    //     vertices[0] = sf::Vertex(sf::Vector2f(m_x, m_y + m_height / 2 + 0.2 * i * (m_height - 10) / 4), sf::Color::Red);
    //     vertices[1] = sf::Vertex(sf::Vector2f(m_x + m_width, m_y + m_height / 2 + 0.2 * i * (m_height - 10) / 4), sf::Color::Red);
    //
    //     target.draw(vertices, 2, sf::PrimitiveType::Lines, state);
    // }

    drawXPoints(target, state);
    drawYPoints(target, state);

    if (m_showSamplers)
        drawSamplers(target, state);
}
