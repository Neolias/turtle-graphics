#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QPointF>
#include <QQmlEngine>
#include <QVector>

class Obstacle;

/**
 * @brief This class represents a canvas where obstacles can be generated and managed.
 *
 * This class allows for obstacle generation, clearing, and access to the obstacle count and properties.
 * The canvas size is adjustable, and the obstacles are generated randomly while avoiding overlap with a turtle.
 */
class Canvas : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @brief The total number of obstacles in the canvas.
     *
     * This property is read-only and notifies when the list of obstacles changes.
     */
    Q_PROPERTY(int obstacle_count READ obstacle_count NOTIFY obstacles_changed FINAL)

    /**
     * @brief The width of the canvas.
     *
     * This property can be read and written. Changes are notified through the width_changed signal.
     */
    Q_PROPERTY(qreal width READ width WRITE set_width NOTIFY width_changed FINAL)

    /**
     * @brief The height of the canvas.
     *
     * This property can be read and written. Changes are notified through the height_changed signal.
     */
    Q_PROPERTY(qreal height READ height WRITE set_height NOTIFY height_changed FINAL)

public:
    /**
     * @brief Constructs a Canvas object with optional parent.
     * @param parent The parent QObject, defaults to nullptr.
     */
    explicit Canvas(QObject *parent = nullptr);

    /**
     * @brief Destructor for the Canvas object, which clears all obstacles.
     */
    ~Canvas();

    /**
     * @brief Returns the width of the canvas.
     * @return The width of the canvas.
     */
    qreal width() const { return m_width; }

    /**
     * @brief Returns the height of the canvas.
     * @return The height of the canvas.
     */
    qreal height() const { return m_height; }

    /**
     * @brief Sets the width of the canvas and notifies the change.
     * @param width The new width of the canvas.
     */
    void set_width(qreal width);

    /**
     * @brief Sets the height of the canvas and notifies the change.
     * @param height The new height of the canvas.
     */
    void set_height(qreal height);

    /**
     * @brief Returns the shape of the canvas.
     * @return The shape of the canvas, QPolygonF with the origin at (0,0) and 4 points forming a rectangle.
     */
    QPolygonF get_shape() const;

    /**
     * @brief Generates a specified number of random obstacles while avoiding overlap with a turtle.
     * @param count The number of obstacles to generate.
     * @param turtle_pos The position of the turtle.
     */
    Q_INVOKABLE void generate_obstacles(int count, const QPointF& turtle_pos);

    /**
     * @brief Clears all obstacles from the canvas.
     */
    Q_INVOKABLE void clear_obstacles();

    /**
     * @brief Retrieves the points of a specific obstacle.
     * @param index The index of the obstacle.
     * @return A QVariantList of the obstacle's points.
     */
    Q_INVOKABLE QVariantList get_obstacle_points(int index) const;

    /**
     * @brief Retrieves the color of a specific obstacle.
     * @param index The index of the obstacle.
     * @return A QString representing the color of the obstacle in hexadecimal format.
     */
    Q_INVOKABLE QString get_obstacle_color(int index) const;

    /**
     * @brief Returns the total number of obstacles currently in the canvas.
     * @return The number of obstacles.
     */
    int obstacle_count() const { return m_obstacles.size(); }

    /**
     * @brief Returns a reference to the list of obstacles.
     * @return A reference to the vector of obstacles.
     */
    const QVector<Obstacle*>& get_obstacles() const { return m_obstacles; }

signals:
    /**
     * @brief Signal emitted when the list of obstacles has changed.
     */
    void obstacles_changed();

    /**
     * @brief Signal emitted when the width of the canvas has changed.
     */
    void width_changed();

    /**
     * @brief Signal emitted when the height of the canvas has changed.
     */
    void height_changed();

private:
    QVector<Obstacle*> m_obstacles; ///< List of obstacles in the canvas
    qreal m_width; ///< The width of the canvas
    qreal m_height; ///< The height of the canvas

    /**
     * @brief Checks if an obstacle overlaps with the turtle area.
     * @param obstacle The obstacle to check.
     * @param turtle_pos The position of the turtle.
     * @param turtle_width The width of the turtle.
     * @param turtle_height The height of the turtle.
     * @return True if the obstacle overlaps with the turtle, otherwise false.
     */
    bool check_turtle_overlap(const Obstacle* obstacle, const QPointF& turtle_pos,
                              qreal turtle_width, qreal turtle_height) const;

    /**
     * @brief Creates a random obstacle with a random size and color.
     * @return A pointer to a new randomly generated obstacle.
     */
    Obstacle* create_random_obstacle() const;
};

#endif // CANVAS_H
