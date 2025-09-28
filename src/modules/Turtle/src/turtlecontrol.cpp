#include "turtlecontrol.h"
#include <QLineF>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QRandomGenerator64>
#include <QtMath>
#include "canvas.hpp"
#include "obstacle.hpp"

// Correlation between degrees and radians
constexpr float DEGREES_TO_RADIANS = M_PI / 180.0f;

TurtleControl::TurtleControl(QObject *parent)
    : QObject(parent)
    , position_(QPointF(450.f, 450.f))
    , rotation_(0.f)
    , speed_(200.f)
    , arc_segments_(50.f)
    , b_pen_down_(true)
    , pen_radius_(3.f)
    , pen_color_(Qt::black)
    , lines_(QVector<Line>())
    , current_anim_(nullptr)
    , canvas_(nullptr)
    , previous_position_(position_)
    , previous_rotation_(rotation_)
    , shape_(QPolygonF())
{
    update_shape();
}

void TurtleControl::set_position(QPointF position)
{
    if (position_ == position)
        return;

    position_ = position;
    update_shape(position - position_);
    emit position_changed();
}

void TurtleControl::set_rotation(float rotation)
{
    if (rotation_ == rotation)
        return;

    if (rotation >= 360.f) {
        rotation_ = fmod(rotation, 360.f);
    } else if (rotation <= 0.f) {
        rotation_ = fmod(360.f + rotation, 360.f);
    } else {
        rotation_ = rotation;
    }

    emit rotation_changed();
}

void TurtleControl::set_pen_down(bool b_pen_down)
{
    if (b_pen_down_ == b_pen_down)
        return;

    b_pen_down_ = b_pen_down;
    if (b_pen_down_) {
        previous_position_ = position_;
    }
    emit pen_down_changed();
}

void TurtleControl::set_pen_radius(float radius)
{
    radius = std::max(1.0f, std::min(9.0f, radius));
    if (pen_radius_ == radius)
        return;

    pen_radius_ = radius;
    update_shape();
    emit pen_radius_changed();
}

void TurtleControl::set_pen_color(const QColor &color)
{
    if (pen_color_ != color) {
        pen_color_ = color;
        emit pen_color_changed();
    }
}

void TurtleControl::set_current_anim(QParallelAnimationGroup *anim)
{
    if (current_anim_) {
        current_anim_->stop();
    }
    current_anim_ = anim;
}

Line TurtleControl::get_line(int index) const
{
    if (index >= 0 && index < lines_.size()) {
        return lines_[index];
    }
    return Line(); // Return a default-initialized line
}

QPointF TurtleControl::get_forward_vector() const
{
    const float forward_rotation = rotation_ - 90.f;
    return QPointF(qCos(qDegreesToRadians(forward_rotation)),
                   qSin(qDegreesToRadians(forward_rotation)));
}

QPointF TurtleControl::get_right_vector() const
{
    return QPointF(qCos(qDegreesToRadians(rotation_)), qSin(qDegreesToRadians(rotation_)));
}

void TurtleControl::update_shape(const QPointF &translation_vector)
{
    // The shape is updated either during movement (translation)
    if (translation_vector != QPointF()) {
        shape_.translate(translation_vector);
        return;
    }

    // Or when the pen radius is changed (new shape)
    // Not both at the same time
    QPolygonF polygon;
    const int segments = 10;
    for (int i = 0; i < segments; ++i) {
        float angle = 2 * M_PI * i / segments; // Angle in radians
        float x = position_.x() + pen_radius_ * cos(angle);
        float y = position_.y() + pen_radius_ * sin(angle);
        polygon << QPointF(x, y);
    }
    shape_ = polygon;
}

void TurtleControl::set_lines(const QVector<Line> &lines)
{
    if (lines.empty()) {
        return;
    }
    lines_ = lines; // Replace current lines with the passed
    previous_position_ = position_;
    emit lines_changed(); // Notify QML or other listeners that lines have changed
    
}

QVector<Line> TurtleControl::get_lines() const
{
    return lines_; 
}

bool TurtleControl::is_moving() const
{
    return current_anim_ && current_anim_->state() == QAbstractAnimation::State::Running;
}

bool TurtleControl::test_collision()
{
    QObject *hit_object = nullptr;
    QPolygonF hit_polygon;

    if (canvas_) {
        const QPolygonF turtle_polygon = get_shape();
        const QPolygonF canvas_polygon = canvas_->get_shape();
        if (canvas_polygon.intersects(turtle_polygon)) {
            QVector<Obstacle *> obstacles = canvas_->get_obstacles();
            for (const auto &obstacle : obstacles) {
                const QPolygonF polygon = obstacle->get_points();
                const float bounding_radius = obstacle->get_bounding_radius();
                const float distance_to_center = QLineF(position_, obstacle->get_position()).length();
                if (bounding_radius >= distance_to_center && polygon.intersects(turtle_polygon)) {
                    hit_object = obstacle;
                    hit_polygon = polygon;
                    break;
                }
            }
        } else {
            hit_object = canvas_;
            hit_polygon = canvas_polygon;
        }
    }

    if (hit_object) {
        set_current_anim(nullptr);
        set_position(previous_position_);
        set_rotation(previous_rotation_);

        emit on_movement_completed(MovementResult::kBlocked);
        emit on_collision(hit_object, hit_polygon);
        return true;
    }

    return false;
}

void TurtleControl::update_lines()
{
    if (previous_position_ == position_) {
        return;
    }

    if (b_pen_down_) {
        Line new_line = {previous_position_,
                         position_,
                         pen_color_,
                         pen_radius_}; // Create a new line at the start of the movement
        lines_.append(new_line);       // Append the new line at the start
        emit lines_changed();          // Notify QML to redraw
    }
}

void TurtleControl::anim_value_changed()
{
    if (!current_anim_) {
        return;
    }

    // if (current_anim_->animationCount() > 1) {
    //     // Do stuff here
    // }

    if (!test_collision()) {
        update_lines();
        previous_position_ = position_;
        previous_rotation_ = rotation_;
    }
}

void TurtleControl::anim_state_changed(QAbstractAnimation::State new_state,
                                       QAbstractAnimation::State old_state)
{
    if (new_state == old_state) {
        return;
    }

    switch (new_state) {
    case QAbstractAnimation::State::Paused:
        emit on_movement_completed(MovementResult::kPaused);
        break;
    default:
    case QAbstractAnimation::State::Stopped:
    case QAbstractAnimation::State::Running:
        break;
    }
}

void TurtleControl::anim_finished()
{
    set_current_anim(nullptr);
    emit on_movement_completed(MovementResult::kSuccess);
}

float TurtleControl::turn(float degrees)
{
    if (is_moving()) {
        emit on_movement_completed(MovementResult::kFailure);
        return 0.001f;
    }

    const float new_rotation = rotation_ + degrees;
    previous_rotation_ = new_rotation;
    set_rotation(new_rotation);
    emit on_movement_completed(MovementResult::kSuccess);
    return 0.001f;
}

float TurtleControl::forward(float distance)
{
    if (is_moving()) {
        emit on_movement_completed(MovementResult::kFailure);
        return 0.001f;
    }

    const QPointF new_position = position_ + get_forward_vector() * distance;
    const float duration = QLineF(position_, new_position).length() * 1000.f / speed_;

    // Creating an animation group that will play multiple animations
    QParallelAnimationGroup *group = new QParallelAnimationGroup;

    // Animating the forward movement
    QPropertyAnimation *anim = new QPropertyAnimation(this, "position", this);
    anim->setDuration(duration);
    anim->setStartValue(position_);
    anim->setEndValue(new_position);
    group->addAnimation(anim);

    // Handling animation progress
    connect(anim, &QPropertyAnimation::valueChanged, this, &TurtleControl::anim_value_changed);
    connect(group, &QParallelAnimationGroup::stateChanged, this, &TurtleControl::anim_state_changed);
    connect(group, &QParallelAnimationGroup::finished, this, &TurtleControl::anim_finished);

    current_anim_ = group;
    group->start(QParallelAnimationGroup::DeleteWhenStopped);
    return duration;
}

float TurtleControl::arc(float radius, float degrees)
{
    if (is_moving()) {
        emit on_movement_completed(MovementResult::kFailure);
        return 0.001f;
    }

    if (radius < 0.001f) {
        anim_finished();
        return 0.001f;
    }

    // Correlation between the arc and a full circle
    const float arc_factor = std::abs(degrees) / 360.f;
    const float path_length = arc_factor * 2 * M_PI * radius;
    const float duration = path_length / speed_ * 1000.f;
    const float rotation_radians = rotation_ * DEGREES_TO_RADIANS;
    const float new_rotation = rotation_ + degrees;
    const float degrees_radians = degrees * DEGREES_TO_RADIANS;
    const float new_rotation_radians = new_rotation * DEGREES_TO_RADIANS;
    const int b_move_clockwise = degrees > 0.f ? -1 : 1;
    const QPointF new_position = position_
                                 + b_move_clockwise * radius
                                       * (QPointF(qCos(new_rotation_radians),
                                                  qSin(new_rotation_radians))
                                          - get_right_vector());

    // Creating an animation group that will play multiple animations
    QParallelAnimationGroup *group = new QParallelAnimationGroup;

    // Rotation anim
    QPropertyAnimation *rotation_anim = new QPropertyAnimation(this, "rotation", this);
    rotation_anim->setDuration(duration);
    rotation_anim->setStartValue(rotation_);
    rotation_anim->setEndValue(new_rotation);
    group->addAnimation(rotation_anim);

    // Position anim
    QPropertyAnimation *position_anim = new QPropertyAnimation(this, "position", this);
    position_anim->setDuration(duration);
    position_anim->setStartValue(position_);
    // Setting position values describing a circular movement
    const float step_delta = 1.f / (arc_segments_ * arc_factor);
    for (float step = 0.f; step < 1.f; step += step_delta) {
        const float rotation_delta = rotation_radians + degrees_radians * step;
        const QPointF position_delta = b_move_clockwise * radius
                                       * (QPointF(qCos(rotation_delta), qSin(rotation_delta))
                                          - get_right_vector());
        position_anim->setKeyValueAt(step, position_ + position_delta);
    }
    position_anim->setEndValue(new_position);
    group->addAnimation(position_anim);

    // Handling animation progress
    connect(position_anim,
            &QPropertyAnimation::valueChanged,
            this,
            &TurtleControl::anim_value_changed);
    // Uncomment belowto signal rotation updates
    // connect(rotation_anim,
    //         &QPropertyAnimation::valueChanged,
    //         this,
    //         &TurtleControl::anim_value_changed);
    connect(group, &QParallelAnimationGroup::stateChanged, this, &TurtleControl::anim_state_changed);
    connect(group, &QParallelAnimationGroup::finished, this, &TurtleControl::anim_finished);

    current_anim_ = group;
    group->start(QParallelAnimationGroup::DeleteWhenStopped);
    return duration;
}

void TurtleControl::on_clicked()
{
    // Generates an angle between 30 and 360
    // Positive or negative
    const int b_positive_num = (QRandomGenerator::global()->bounded(2)) * 2 - 1;
    const int random_angle = b_positive_num
                             * std::max(30, QRandomGenerator::global()->bounded(361));

    const int random_action = QRandomGenerator::global()->bounded(2);
    if (random_action) {
        // Generates a distance between 20 and 200
        const int random_distance = std::max(20, QRandomGenerator::global()->bounded(201));
        turn(random_angle);
        forward(random_distance);
    } else {
        // Generates a radius between 50 and 100
        const int random_radius = std::max(50, QRandomGenerator::global()->bounded(101));
        arc(random_radius, random_angle);
    }
}

void TurtleControl::reset_state()
{
    TurtleControl::set_pen_down(false);
    // Set the position to (450, 450)
    position_ = QPointF(450, 450);
    emit position_changed();
    TurtleControl::set_pen_down(true);

    rotation_ = 0.0f;
    emit rotation_changed();

    // Clear the lines vector
    lines_.clear();
    emit lines_changed();
    
    // Ensure that the Parser will not get blocked when resetting the state while running a script
    emit on_movement_completed(MovementResult::kSuccess);

}
