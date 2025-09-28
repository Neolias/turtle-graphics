#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QObject>
#include <QColor>
#include <QPolygonF>
#include <QPointF>
#include <QQmlEngine>

/**
 * @class Obstacle
 * @brief The class represents an obstacle in the canvas,
 *
 * This class encapsulates the points, color, and position of an obstacle, and provides methods to manipulate these properties.
 * It also includes functionality for checking whether the obstacle intersects with a given rectangle.
 */
class Obstacle : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @brief Points of the obstacle.
     *
     * A polygon representing the vertices of the obstacle shape.
     */
    Q_PROPERTY(QPolygonF points READ get_points WRITE set_points NOTIFY points_changed)

    /**
     * @brief Color of the obstacle.
     *
     * The color used to draw the obstacle.
     */
    Q_PROPERTY(QColor color READ get_color WRITE set_color NOTIFY color_changed)

    /**
     * @brief Position of the obstacle.
     *
     * The center point of the obstacle in the scene.
     */
    Q_PROPERTY(QPointF position READ get_position WRITE set_position NOTIFY position_changed)

public:
    /**
     * @brief Default constructor for Obstacle.
     *
     * Constructs an obstacle with default properties (empty polygon, red color).
     *
     * @param parent The parent object, default is nullptr.
     */
    explicit Obstacle(QObject *parent = nullptr);

    /**
     * @brief Constructs an Obstacle with specific properties.
     *
     * @param points The points defining the obstacle's shape.
     * @param color The color of the obstacle.
     * @param parent The parent object, default is nullptr.
     */
    Obstacle(const QPolygonF& points, const QColor& color, QObject *parent = nullptr);

    /**
     * @brief Gets the points defining the obstacle's shape.
     *
     * @return The points of the obstacle as a QPolygonF.
     */
    QPolygonF get_points() const { return m_points; }

    /**
     * @brief Gets the color of the obstacle.
     *
     * @return The color of the obstacle as a QColor.
     */
    QColor get_color() const { return m_color; }

    /**
     * @brief Gets the position (center point) of the obstacle.
     *
     * @return The position as a QPointF.
     */
    QPointF get_position() const { return m_position; }

    /**
     * @brief Gets the bounding radius of the obstacle.
     * Gets the distance from the center to the furthest point of the obstacle's bounding rect.
     * Used in collision calculations.
     * @return The bounding radius.
     */
    float get_bounding_radius() const { return m_bounding_radius; }

    /**
     * @brief Sets the points defining the obstacle's shape.
     *
     * This will update the obstacle's position based on the new points.
     *
     * @param points The new points for the obstacle.
     */
    void set_points(const QPolygonF& points);

    /**
     * @brief Sets the color of the obstacle.
     *
     * @param color The new color for the obstacle.
     */
    void set_color(const QColor& color);

    /**
     * @brief Sets the position (center point) of the obstacle.
     *
     * This will translate the points of the obstacle to maintain the shape, but move it to the new position.
     *
     * @param pos The new position for the obstacle.
     */
    void set_position(const QPointF& pos);

    /**
     * @brief Checks if the obstacle intersects with a given rectangle.
     *
     * This is done by checking whether the bounding box of the obstacle intersects the given rectangle.
     *
     * @param rect The rectangle to check for intersection.
     * @return True if the obstacle intersects the rectangle, false otherwise.
     */
    bool intersects(const QRectF& rect) const;

signals:
    /**
     * @brief Emitted when the points of the obstacle change.
     */
    void points_changed();

    /**
     * @brief Emitted when the color of the obstacle changes.
     */
    void color_changed();

    /**
     * @brief Emitted when the position of the obstacle changes.
     */
    void position_changed();

private:
    QPolygonF m_points; /**< The points defining the obstacle's shape. */
    QColor m_color;     /**< The color of the obstacle. */
    QPointF m_position; /**< The position (center point) of the obstacle. */
    float m_bounding_radius; /**< The distance from the center to the furthest point of the obstacle polygon.

    /**
     * @brief Updates the position of the obstacle based on its points.
     *
     * This function calculates the centroid (average) of the points to determine the position.
     */
    void update_position();

    /**
     * @brief Updates the bounding radius.
     */
    void update_bounding_radius();
};

#endif
