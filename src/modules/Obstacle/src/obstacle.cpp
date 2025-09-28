#include "obstacle.hpp"

#include <QLineF>

Obstacle::Obstacle(QObject *parent)
    : QObject(parent)
    , m_color(Qt::red)
    , m_bounding_radius(0.001f)
{
    update_bounding_radius();
}

Obstacle::Obstacle(const QPolygonF &points, const QColor &color, QObject *parent)
    : QObject(parent)
    , m_points(points)
    , m_color(color)
    , m_bounding_radius(0.001f)
{
    update_position();
    update_bounding_radius();
}

void Obstacle::set_points(const QPolygonF& points)
{
    if (m_points != points) {
        m_points = points;
        update_position();
        emit points_changed();
    }
}

void Obstacle::set_color(const QColor& color)
{
    if (m_color != color) {
        m_color = color;
        emit color_changed();
    }
}

void Obstacle::set_position(const QPointF& pos)
{
    if (m_position != pos) {
        QPointF delta = pos - m_position;
        m_position = pos;
        m_points.translate(delta);
        emit position_changed();
        emit points_changed();
    }
}

bool Obstacle::intersects(const QRectF& rect) const
{
    return m_points.boundingRect().intersects(rect);
}

void Obstacle::update_position()
{
    if (!m_points.isEmpty()) {
        QPointF sum(0, 0);
        for (const QPointF& point : m_points) {
            sum += point;
        }
        m_position = sum / m_points.size();
    }
}

void Obstacle::update_bounding_radius()
{
    const QRectF bounding_rect = m_points.boundingRect();
    m_bounding_radius = QLineF(bounding_rect.topLeft(), bounding_rect.bottomRight()).length()
                        * 0.5f;
}
