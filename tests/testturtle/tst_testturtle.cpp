#include <QtTest>
#include "canvas.hpp"
#include "obstacle.hpp"
#include "turtlecontrol.h"

class TestTurtle : public QObject
{
    Q_OBJECT

public:
    TestTurtle();
    ~TestTurtle();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_constructor_getters();
    void test_forward();
    void test_turn();
    void test_circle();
    void test_arc();
    void test_collision();

private:
    Canvas *canvas_;
    TurtleControl *turtle_;
    QPointF initial_turtle_position_;
};

TestTurtle::TestTurtle() {}

TestTurtle::~TestTurtle() {}

void TestTurtle::initTestCase()
{
    canvas_ = new Canvas();
    canvas_->set_width(800.0);
    canvas_->set_height(600.0);
    turtle_ = new TurtleControl();
    initial_turtle_position_ = turtle_->position();
    turtle_->set_canvas(canvas_);
}

void TestTurtle::cleanupTestCase()
{
    delete turtle_;
    delete canvas_;
}

void TestTurtle::test_constructor_getters()
{
    QCOMPARE(turtle_->position(), QPointF(450, 450));
    QCOMPARE(turtle_->rotation(), 0.f);
}

void TestTurtle::test_forward()
{
    turtle_->set_position(initial_turtle_position_);
    turtle_->set_speed(1000.f);
    const float distance = 150.f;
    MovementResult result;
    const QPointF expected_position = turtle_->position()
                                      + turtle_->get_forward_vector() * distance;

    // Set up a spy and call the command
    QSignalSpy spy(turtle_, &TurtleControl::on_movement_completed);
    QVERIFY(spy.isValid());
    const float duration = turtle_->forward(distance);
    spy.wait(duration * 1.1f);

    QCOMPARE(spy.count(), 1); // making sure the signal was emitted exactly one time
    QList<QVariant> arguments = spy.takeFirst(); // take the first signal
    QVERIFY(!arguments.isEmpty());
    result = (MovementResult) arguments.at(0).value<MovementResult>();

    QCOMPARE(result, MovementResult::kSuccess);
    QCOMPARE(turtle_->position().toPoint(), expected_position.toPoint());
}

void TestTurtle::test_turn()
{
    turtle_->set_position(initial_turtle_position_);
    const float degrees = 15.f;
    MovementResult result;
    const float expected_rotation = turtle_->rotation() + degrees;

    // Set up a spy and call the command
    QSignalSpy spy(turtle_, &TurtleControl::on_movement_completed);
    QVERIFY(spy.isValid());
    const float duration = turtle_->turn(degrees);
    spy.wait(duration * 1.1f);

    QCOMPARE(spy.count(), 1); // making sure the signal was emitted exactly one time
    QList<QVariant> arguments = spy.takeFirst(); // take the first signal
    QVERIFY(!arguments.isEmpty());
    result = (MovementResult) arguments.at(0).value<MovementResult>();

    QCOMPARE(result, MovementResult::kSuccess);
    QCOMPARE(turtle_->rotation(), expected_rotation);
}

void TestTurtle::test_circle()
{
    turtle_->set_position(initial_turtle_position_);
    turtle_->set_speed(3500.f);
    const float radius = 100.f;
    MovementResult result;
    const QPointF expected_position = turtle_->position();
    const float expected_rotation = turtle_->rotation();

    // Set up a spy and call the command
    QSignalSpy spy(turtle_, &TurtleControl::on_movement_completed);
    QVERIFY(spy.isValid());
    const float duration = turtle_->arc(radius);
    spy.wait(duration * 1.1f);

    QCOMPARE(spy.count(), 1); // making sure the signal was emitted exactly one time
    QList<QVariant> arguments = spy.takeFirst(); // take the first signal
    QVERIFY(!arguments.isEmpty());
    result = (MovementResult) arguments.at(0).value<MovementResult>();

    QCOMPARE(result, MovementResult::kSuccess);
    QCOMPARE(turtle_->position().toPoint(), expected_position.toPoint());
    QCOMPARE(turtle_->rotation(), expected_rotation);
}

void TestTurtle::test_arc()
{
    turtle_->set_position(initial_turtle_position_);
    turtle_->set_speed(2000.f);
    const float radius = 100.f;
    const float degrees = 120.f;
    MovementResult result;
    const float expected_rotation = turtle_->rotation() + degrees;
    const float degrees_radians = degrees * (M_PI / 180.0f);
    const float expected_rotation_radians = expected_rotation * (M_PI / 180.0f);
    const int b_move_clockwise = degrees > 0.f ? -1 : 1;
    const QPointF expected_position = turtle_->position()
                                      + b_move_clockwise * radius
                                            * (QPointF(qCos(expected_rotation_radians),
                                                       qSin(expected_rotation_radians))
                                               - turtle_->get_right_vector());

    // Set up a spy and call the command
    QSignalSpy spy(turtle_, &TurtleControl::on_movement_completed);
    QVERIFY(spy.isValid());
    const float duration = turtle_->arc(radius, degrees);
    spy.wait(duration * 1.1f);

    QCOMPARE(spy.count(), 1); // making sure the signal was emitted exactly one time
    QList<QVariant> arguments = spy.takeFirst(); // take the first signal
    QVERIFY(!arguments.isEmpty());
    result = (MovementResult) arguments.at(0).value<MovementResult>();

    QCOMPARE(result, MovementResult::kSuccess);
    QCOMPARE(turtle_->position().toPoint(), expected_position.toPoint());
    QCOMPARE(turtle_->rotation(), expected_rotation);
}

void TestTurtle::test_collision()
{
    turtle_->set_position(initial_turtle_position_);
    turtle_->set_speed(1000.f);
    const float distance = 150.f;
    const float test_tolerance = 20.f; // equal to SIZE_RANGE in Canvas::create_random_obstacle()

    canvas_->generate_obstacles(1, initial_turtle_position_);
    QCOMPARE(canvas_->obstacle_count(), 1);

    Obstacle *obstacle = canvas_->get_obstacles()[0];
    const QPointF new_obstacle_position = initial_turtle_position_
                                          + turtle_->get_forward_vector() * distance * 0.5f;
    obstacle->set_position(new_obstacle_position);

    MovementResult result;
    const QPointF expected_position = new_obstacle_position
                                      - turtle_->get_forward_vector()
                                            * obstacle->get_bounding_radius();

    // Set up a spy and call the command
    QSignalSpy spy(turtle_, &TurtleControl::on_movement_completed);
    QVERIFY(spy.isValid());
    const float duration = turtle_->forward(distance);
    spy.wait(duration * 1.1f);

    QCOMPARE(spy.count(), 1); // making sure the signal was emitted exactly one time
    QList<QVariant> arguments = spy.takeFirst(); // take the first signal
    QVERIFY(!arguments.isEmpty());
    result = (MovementResult) arguments.at(0).value<MovementResult>();

    QCOMPARE(result, MovementResult::kBlocked);
    QVERIFY(turtle_->position().toPoint() != expected_position.toPoint());
    const float distance_to_obstacle = QLineF(expected_position, turtle_->position()).length();
    QCOMPARE_LE(distance_to_obstacle, test_tolerance);
}

QTEST_MAIN(TestTurtle)

#include "tst_testturtle.moc"
