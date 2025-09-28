#ifndef TURTLECONTROL_H
#define TURTLECONTROL_H

#include <QColor>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QPoint>
#include <QPolygon>
#include <QQmlEngine>

class Canvas;
Q_DECLARE_OPAQUE_POINTER(Canvas *);
class QLine;

/**
 * @brief A structure representing a line segment with customizable attributes.
 */
struct Line
{
    Line(const QPointF &start = QPointF(),
         const QPointF &end = QPointF(),
         const QColor &color = QColor(),
         float width = 1.f)
        : start_(start)
        , end_(end)
        , color_(color)
        , width_(width)
    {}

    QPointF start_; ///< The starting point of the line segment.
    QPointF end_;   ///< The ending point of the line segment.
    QColor color_;  ///< The color of the line.
    float width_;   ///< The width of the line.
    Q_GADGET
    Q_PROPERTY(QPointF start MEMBER start_)
    Q_PROPERTY(QPointF end MEMBER end_)
    Q_PROPERTY(QColor color MEMBER color_)
    Q_PROPERTY(float width MEMBER width_)
};

/**
 * @brief Enum to represent the result of turtle movement operations.
 * 
 * This enumeration defines the possible outcomes when attempting to move the turtle. 
 * It helps to indicate the status of movement commands.
 */
enum MovementResult {
    kSuccess = 0, ///< The movement was successful.
    kFailure,     ///< The movement failed due to an unspecified reason.
    kBlocked,     ///< The movement was blocked, possibly by an obstacle.
    kPaused       ///< The movement was paused.
};

/**
 * @brief The TurtleControl class
 * 
 * The TurtleControl class handles the movement and pen properties of a turtle. 
 * It is designed to be used as a QML element, allowing for easy integration 
 * into QML-based applications. The class provides properties for position, 
 * rotation, pen state, radius, and color, along with methods to control 
 * the turtle's movement.
 */
class TurtleControl : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /// @brief The current position of the turtle.
    Q_PROPERTY(QPointF position READ position WRITE set_position NOTIFY position_changed FINAL)

    /// @brief The current rotation angle of the turtle in degrees.
    Q_PROPERTY(float rotation READ rotation WRITE set_rotation NOTIFY rotation_changed FINAL)

    /// @brief Indicates whether the pen is currently down.
    Q_PROPERTY(bool pen_down READ pen_down NOTIFY pen_down_changed FINAL)

    /// @brief The radius of the pen.
    Q_PROPERTY(float pen_radius READ pen_radius NOTIFY pen_radius_changed FINAL)

    /// @brief The color of the pen.
    Q_PROPERTY(QColor pen_color READ pen_color WRITE set_pen_color NOTIFY pen_color_changed FINAL)

    /// @brief The total number of lines drawn by the turtle.
    Q_PROPERTY(int line_count READ line_count NOTIFY lines_changed FINAL)

public:
    /**
     * @brief Constructs a TurtleControl object.
     * 
     * This constructor initializes the turtle's properties with default values.
     * 
     * @param parent Pointer to the parent QObject. This is used for 
     *               managing the object hierarchy in Qt.
     */
    explicit TurtleControl(QObject *parent = nullptr);

    /// @brief Gets the current position of the turtle.
    /// @return The current position as a QPointF.
    QPointF position() const { return position_; }

    /// @brief Gets the current rotation of the turtle.
    /// @return The current rotation angle in degrees.
    float rotation() const { return rotation_; }

    /// @brief Checks if the pen is down.
    /// @return True if the pen is down, false otherwise.
    bool pen_down() const { return b_pen_down_; }

    /// @brief Gets the current pen radius.
    /// @return The current pen radius.
    float pen_radius() const { return pen_radius_; }

    /// @brief Gets the current pen color.
    /// @return The current pen color as a QColor.
    QColor pen_color() const { return pen_color_; }

    /// @brief Gets the current movement speed of the turtle.
    /// @return The movement speed.
    float get_speed() const { return speed_; }

    /// @brief Gets the current Gets the current number arc segments.
    /// Number of segments needed to draw a full circle using the arc command.
    /// @return The current number of arc segments.
    float get_arc_segments() const { return arc_segments_; }

    /// @brief Sets arc segments.
    /// Sets the number of segments needed to draw a full circle using the arc command.
    /// @param arc_segments The new number of segments.
    void set_arc_segments(float arc_segments) { arc_segments_ = arc_segments; }

    /// @brief Gets the total number of lines drawn by the turtle.
    /// @return The number of lines.
    int line_count() const { return lines_.size(); }

    /// @brief Calculates the forward vector based on the turtle's current rotation.
    /// @return The forward vector as a QPointF.
    QPointF get_forward_vector() const;

    /// @brief Calculates the right vector based on the turtle's current rotation.
    /// @return The right vector as a QPointF.
    QPointF get_right_vector() const;

    /// @brief Returns the shape of the cursor.
    /// Returns a circle with the turtle origin and a radius equal to the pen radius.
    /// @return The circular shape as a QPolygonF.
    const QPolygonF &get_shape() const { return shape_; }

    /// @brief Used to update the turtle shape on position and pen changes.
    /// @param translation_vector Translation vector. Default is (0,0).
    void update_shape(const QPointF &translation_vector = QPointF());

    /**
     * @brief Indicates if the turtle is currently moving.
     * 
     * @return True if the turtle movement animation exists and is being played, false otherwise.
     */
    bool is_moving() const;

    /**
     * @brief Sets the pen color.
     *
     * @param color The new color for the pen.
     */
    Q_INVOKABLE void set_pen_color(const QColor &color);

    /**
     * @brief Retrieves a specific line from the collection.
     * 
     * @param index The index of the line to retrieve.
     * @return The line at the given index, or an empty line if the index is invalid.
     */
    Q_INVOKABLE Line get_line(int index) const;

    /**
     * @brief Sets the canvas.
     *
     * @param canvas The pointer to the canvas.
     */
    Q_INVOKABLE void set_canvas(Canvas *canvas)
    {
        canvas_ = canvas;
    }
   

public slots:
    /// @brief Slot to handle click events.
    /// Triggers a random movement action.
    void on_clicked();

    /// @brief Sets the movement speed of the turtle.
    /// @param speed The new movement speed.
    void set_speed(float speed) { speed_ = std::min(speed,9999.0f); }

    /// @brief Sets the turtle's position.
    /// @param position The new position for the turtle.
    void set_position(QPointF position);

    /// @brief Sets the turtle's rotation.
    /// @param rotation The new rotation angle in degrees.
    void set_rotation(float rotation);

    /**
     * @brief Sets the pen down state.
     * 
     * @param b_pen_down True to set the pen down, false to lift it.
     */
    void set_pen_down(bool b_pen_down);

    /**
     * @brief Sets the pen radius.
     * 
     * @param radius The new radius for the pen.
     */
    void set_pen_radius(float radius);

    /**
     * @brief Rotates the turtle clockwise by a specified number of degrees.
     * 
     * @param degrees The angle in degrees to rotate the turtle. Default value is 30.
     * @return The movement duration.
     */
    float turn(float degrees = 30.f);

    /**
     * @brief Moves the turtle forward by a specified distance.
     * 
     * @param distance The distance to move the turtle forward. Default value is 100.
     * @return The movement duration.
     */
    float forward(float distance = 100.f);

    /**
     * @brief Draws an arc with the turtle.
     * 
     * @param radius The radius of the arc.
     * @param degrees The angle in degrees to draw the arc. Default value is 360 indicating a full circle.
     * @return The duration of the arc movement.
     */
    float arc(float radius, float degrees = 360.f);
    
    /**
   * @brief Replaces the current set of lines and notifies listeners.
   * @param lines A QVector of Line structures representing the new lines.
   */
   
    void set_lines(const QVector<Line> &lines);
    
      /**
     * @brief Public function to get lines.
     * 
     * @param none.
     */
    QVector<Line> get_lines() const;

    /**
     * @brief Resets the turtle to its initial state.
     *
     * Sets the position to (450, 450) and clears the lines vector.
     */
    void reset_state();

signals:
    /**
     * @brief Emitted when the position of the turtle changes.
     */
    void position_changed();

    /**
     * @brief Emitted when the rotation of the turtle changes.
     */
    void rotation_changed();

    /**
     * @brief Emitted when the pen state changes.
     */
    void pen_down_changed();

    /**
     * @brief Emitted when the pen radius changes.
     */
    void pen_radius_changed();

    /**
     * @brief Emitted when the pen color changes.
     */
    void pen_color_changed();

    /// @brief Emitted when the lines vector changes.
    void lines_changed();
    
    /// @brief Emitted when a movement operation is completed.
    /// @param movement_result The result of the movement operation.
    void on_movement_completed(MovementResult movement_result);

    /// @brief Emitted when on collision event.
    /// Emitted when a collision with obstacle or the canvas borders occurs.
    /// @param hit_object The object collided with.
    /// @param hit_polygon The shape of the collided object.
    void on_collision(QObject *hit_object, const QPolygonF &hit_polygon);

private:
    QPointF position_;                 ///< Current position of the turtle.
    float rotation_;                   ///< Current rotation of the turtle.
    float speed_;                      ///< Speed at which the turtle moves.
    float arc_segments_; ///< Number of segments needed to draw a full circle using the arc command.
    bool b_pen_down_;                  ///< Indicates if the pen is down.
    float pen_radius_;                 ///< Radius of the pen.
    QColor pen_color_;                 ///< Color of the pen.
    QVector<Line> lines_;              ///< Collection of lines drawn by the turtle.
    QParallelAnimationGroup *current_anim_; ///< Animation that is currently being played.
    Canvas *canvas_; ///< Canvas module used to retrieve canvas properties and the list of obstacles.
    QPointF previous_position_; ///< Position of the turtle before the animation was updated.
    float previous_rotation_;   ///< Rotation of the turtle before the animation was updated.
    QPolygonF shape_;           ///< Shape of the turtle cursor.

    /// @brief Handles changes in animation values.
    void anim_value_changed();
    /**
     * @brief Handles changes in animation state.
     * 
     * @param new_state The new state of the animation.
     * @param old_state The previous state of the animation.
     */
    void anim_state_changed(QAbstractAnimation::State new_state,
                            QAbstractAnimation::State old_state);

    /// @brief Handles the completion of an animation.
    void anim_finished();

    /**
     * @brief Updates the current animation pointer.
     * 
     * Stops playing the current animation and replaces it with a new one.
     * The old animation object will be automatically deleted.
     * 
     * @param anim The animation that will replace the current animation.  
     */
    void set_current_anim(QParallelAnimationGroup *anim);

    /// @brief Handles collisions with obstacles and canvas borders.
    /// @return Boolean indicating test result. True if collides.
    bool test_collision();

    /// @brief Adds a new line and emits the lines_changed signal.
    void update_lines();
};

#endif // TURTLECONTROL_H
