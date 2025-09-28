#include "canvas.hpp"
#include "obstacle.hpp"
#include <QRandomGenerator64>

Canvas::Canvas(QObject *parent)
    : QObject(parent)
    , m_width(0)
    , m_height(0)
{
}

Canvas::~Canvas()
{
    clear_obstacles();
}

void Canvas::set_width(qreal width)
{
    if (m_width != width) {
        m_width = width;
        emit width_changed();
    }
}

void Canvas::set_height(qreal height)
{
    if (m_height != height) {
        m_height = height;
        emit height_changed();
    }
}

void Canvas::generate_obstacles(int count, const QPointF& turtle_pos)
{
    const int MAX_ATTEMPTS = 100;
    const qreal TURTLE_WIDTH = 25.0;
    const qreal TURTLE_HEIGHT = 25.0;

    for (int i = 0; i < count;) {
        int attempts = 0;
        while (attempts < MAX_ATTEMPTS) {
            Obstacle* obstacle = create_random_obstacle();

            if (!check_turtle_overlap(obstacle, turtle_pos, TURTLE_WIDTH, TURTLE_HEIGHT)) {
                m_obstacles.append(obstacle);
                i++;
                break;
            }

            delete obstacle;
            attempts++;
        }

        if (attempts >= MAX_ATTEMPTS) {
            i++;
        }
    }

    emit obstacles_changed();
}

Obstacle* Canvas::create_random_obstacle() const
{
    const QVector<QColor> color_palette = {
        Qt::red, Qt::blue, Qt::green, Qt::cyan,
        Qt::magenta, Qt::yellow, Qt::darkRed,
        Qt::darkGreen, Qt::darkBlue
    };

    enum class ShapeType {
        Triangle,
        Rectangle,
        Pentagon
    };

    const qreal MIN_SIZE = 25.0;
    const qreal SIZE_RANGE = 20.0;

    // Random center position
    qreal x = QRandomGenerator::global()->generateDouble() * m_width;
    qreal y = QRandomGenerator::global()->generateDouble() * m_height;

    // Random size
    qreal size = MIN_SIZE + QRandomGenerator::global()->generateDouble() * SIZE_RANGE;

    // Random rotation angle
    qreal rotation_angle = QRandomGenerator::global()->generateDouble() * 2 * M_PI;

    // Select a random shape
    ShapeType shape_type = static_cast<ShapeType>(
        QRandomGenerator::global()->bounded(3)
        );

    // Determine the number of sides
    int sides = 3; // Default: Triangle
    if (shape_type == ShapeType::Rectangle) sides = 4;
    else if (shape_type == ShapeType::Pentagon) sides = 5;

    QPolygonF polygon;
    const qreal angle_step = 2 * M_PI / sides;
    for (int i = 0; i < sides; ++i) {
        qreal angle = i * angle_step + rotation_angle;
        polygon << QPointF(
            x + size * std::cos(angle),
            y + size * std::sin(angle)
            );
    }

    // Select a random color
    QColor color = color_palette[
        QRandomGenerator::global()->bounded(color_palette.size())
    ];

    return new Obstacle(polygon, color, const_cast<Canvas*>(this));
}

bool Canvas::check_turtle_overlap(const Obstacle* obstacle, const QPointF& turtle_pos,
                                  qreal turtle_width, qreal turtle_height) const
{
    QRectF turtle_area(
        turtle_pos.x() - turtle_width / 2,
        turtle_pos.y() - turtle_height / 2,
        turtle_width,
        turtle_height
        );

    return obstacle->intersects(turtle_area);
}

void Canvas::clear_obstacles()
{
    qDeleteAll(m_obstacles);
    m_obstacles.clear();
    emit obstacles_changed();
}

QVariantList Canvas::get_obstacle_points(int index) const
{
    QVariantList points;
    if (index >= 0 && index < m_obstacles.size()) {
        const QPolygonF& polygon = m_obstacles[index]->get_points();
        for (const QPointF& point : polygon) {
            points.append(point.x());
            points.append(point.y());
        }
    }
    return points;
}

QString Canvas::get_obstacle_color(int index) const
{
    if (index >= 0 && index < m_obstacles.size()) {
        return m_obstacles[index]->get_color().name();
    }
    return "#FF0000";
}

QPolygonF Canvas::get_shape() const
{
    return QPolygonF(QRectF(QPointF(0.f, 0.f), QPointF(m_width, m_height)));
}
