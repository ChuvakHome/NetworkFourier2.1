
#include <Fourier/UI/DiscreteDiagram.hpp>

#include <cmath>
#include <cstdlib>

#include <Fourier/Utils.hpp>

static int diagram_point_comparator(const void *x, const void *y) {
    const diagram_point *p1 = static_cast<const diagram_point *>(x);
    const diagram_point *p2 = static_cast<const diagram_point *>(y);

    return p1->x > p2->x ? 1 : (fabs(p1->x - p2->x) <= 0.00001 ? 0 : -1);
}

UIDiscreteDiagram::UIDiscreteDiagram(float width, float height) : m_x(0), m_y(0), m_width(width), m_height(height), m_showDiagram(true), m_points(8), m_groupPointCount(8) {
    for (std::size_t i = 0; i < MAX_DIAGRAM_POINTS_COUNT; ++i)
        m_diagramPoints[i] = { static_cast<float>(i), -1 };

    m_diagramPointsCount = 2;
}

void UIDiscreteDiagram::setSize(const sf::Vector2f &size) {
    m_width = size.x;
    m_height = size.y;
}

sf::Vector2f UIDiscreteDiagram::getSize() const {
    return sf::Vector2f(m_width, m_height);
}

void UIDiscreteDiagram::setShowDiagram(bool flag) {
    m_showDiagram = flag;
}

void UIDiscreteDiagram::setPosition(const sf::Vector2f &pos) {
    m_x = pos.x;
    m_y = pos.y;
}

sf::Vector2f UIDiscreteDiagram::getPosition() const {
    return sf::Vector2f(m_x, m_y);
}

void UIDiscreteDiagram::setDrawMode(std::size_t points, std::size_t pointInGroup) {
    if (points > 0)
        m_points = points;

    if (pointInGroup > 0)
        m_groupPointCount = pointInGroup;
}

void UIDiscreteDiagram::setDiagram(discrete_diagram diagram) {
    if (diagram.diagram_points_count > 0) {
        m_diagramPointsCount = diagram.diagram_points_count <= MAX_DIAGRAM_POINTS_COUNT ? diagram.diagram_points_count : MAX_DIAGRAM_POINTS_COUNT;

        for (std::size_t i = 0; i < m_diagramPointsCount; ++i)
            m_diagramPoints[i] = diagram.diagram_points[i];

        qsort(m_diagramPoints, m_diagramPointsCount, sizeof(diagram_point), diagram_point_comparator);
    }
}

static inline sf::Vertex diagram_point_to_vertex(diagram_point point, float startm_x, float startm_y, float dx, float y_offset, sf::Color color) {
    return sf::Vertex(sf::Vector2f(startm_x + dx * point.x, startm_y + y_offset - (y_offset - 5) * point.y), color);
}

void UIDiscreteDiagram::drawXPoints(sf::RenderTarget &target, sf::RenderStates state) const {
    std::size_t i = 0;
    float x = 0;
    float dx = m_width / m_points;
    float yOffset = m_height / 2;
    sf::Color vertexColor(0xff, 0xff, 0xff, 0xff);

    sf::Vertex vertices2[] = {
        sf::Vertex(sf::Vector2f(m_x + m_width, m_y + yOffset - 7), vertexColor),
        sf::Vertex(sf::Vector2f(m_x + m_width, m_y + yOffset + 8), vertexColor),
    };

    target.draw(vertices2, 2, sf::PrimitiveType::Lines, state);

    while (x < m_width) {
        ++i;

        if (i == m_groupPointCount) {
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

void UIDiscreteDiagram::drawDiagram(sf::RenderTarget &target, sf::RenderStates state) const {
    std::size_t i = 0;
    float x = 0;
    float startX = m_x;
    float dx = m_width / m_points;
    float yOffset = m_height / 2;

    diagram_point prev = { 0.0f, 0.0f };
    sf::Color levelVertexColor(0xc6, 0xc6, 0xc6, 0xcc);

    sf::Vertex vertices[] = {
        sf::Vertex(sf::Vector2f(0.0f, 0.0f), levelVertexColor),
        sf::Vertex(sf::Vector2f(0.0f, 0.0f), levelVertexColor),
    };

    while (x < m_width) {
        diagram_point point = m_diagramPoints[i];
        diagram_point tempPoint = { prev.x, point.y };

        if (is_not_equal(prev.y, point.y)) {
            vertices[0] = diagram_point_to_vertex(prev, startX, m_y, dx, yOffset, levelVertexColor);
            vertices[1] = diagram_point_to_vertex(tempPoint, startX, m_y, dx, yOffset, levelVertexColor);

            target.draw(vertices, 2, sf::PrimitiveType::Lines, state);
        }

        vertices[0] = diagram_point_to_vertex(tempPoint, startX, m_y, dx, yOffset, levelVertexColor);
        vertices[1] = diagram_point_to_vertex(point, startX, m_y, dx, yOffset, levelVertexColor);

        target.draw(vertices, 2, sf::PrimitiveType::Lines, state);

        x += dx * (point.x - prev.x);

        ++i;

        if (i == m_diagramPointsCount) {
            startX = m_x + x;
            i = 0;

            prev = { 0.0f, point.y };
        }
        else
            prev = point;
    }
}

void UIDiscreteDiagram::draw(sf::RenderTarget &target, sf::RenderStates state) const {
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

    drawXPoints(target, state);
}
