#include <QtTest>
#include "canvas.hpp"
#include "obstacle.hpp"

class TestCanvas : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();        // Set up test case
    void cleanupTestCase();     // Clean up after all tests
    void test_initial_state();  // Verify initial state of Canvas
    void test_set_dimensions(); // Test width and height setters
    void test_generate_obstacles(); // Test obstacle generation
    void test_clear_obstacles(); // Test obstacle clearing
    void test_obstacle_properties(); // Test obstacle color and points

private:
    Canvas* m_canvas; // Pointer to Canvas for testing
};

void TestCanvas::initTestCase()
{
    m_canvas = new Canvas();
}

void TestCanvas::cleanupTestCase()
{
    delete m_canvas;
}

void TestCanvas::test_initial_state()
{
    QCOMPARE(m_canvas->width(), 0.0);
    QCOMPARE(m_canvas->height(), 0.0);
    QCOMPARE(m_canvas->obstacle_count(), 0);
}

void TestCanvas::test_set_dimensions()
{
    QSignalSpy widthSpy(m_canvas, &Canvas::width_changed);
    QSignalSpy heightSpy(m_canvas, &Canvas::height_changed);

    m_canvas->set_width(800.0);
    m_canvas->set_height(600.0);

    QCOMPARE(m_canvas->width(), 800.0);
    QCOMPARE(m_canvas->height(), 600.0);

    QCOMPARE(widthSpy.count(), 1);
    QCOMPARE(heightSpy.count(), 1);
}

void TestCanvas::test_generate_obstacles()
{
    QPointF turtlePos(400.0, 300.0);
    int obstacleCount = 1000;

    m_canvas->generate_obstacles(obstacleCount, turtlePos);

    QCOMPARE(m_canvas->obstacle_count(), obstacleCount);

    // Verify no obstacles overlap with the turtle
    QRectF turtleArea(turtlePos.x() - 12.5, turtlePos.y() - 12.5, 25.0, 25.0);
    for (const Obstacle* obstacle : m_canvas->get_obstacles()) {
        QVERIFY(!obstacle->intersects(turtleArea));
    }
}

void TestCanvas::test_clear_obstacles()
{
    QPointF turtlePos(400.0, 300.0);
    int obstacleCount = 1000;

    QSignalSpy spy(m_canvas, &Canvas::obstacles_changed);
    m_canvas->clear_obstacles();

    QCOMPARE(m_canvas->obstacle_count(), 0);
    QCOMPARE(spy.count(), 1);
}

void TestCanvas::test_obstacle_properties()
{
    QPointF turtlePos(400.0, 300.0);
    int obstacleCount = 1;

    m_canvas->generate_obstacles(obstacleCount, turtlePos);
    QVERIFY(m_canvas->obstacle_count() > 0);

    QVariantList points = m_canvas->get_obstacle_points(0);
    QVERIFY(!points.isEmpty());

    QString color = m_canvas->get_obstacle_color(0);
    QVERIFY(!color.isEmpty());

    QColor parsedColor(color);
    QVERIFY(parsedColor.isValid());
}

QTEST_MAIN(TestCanvas)
#include "tst_testcanvas.moc"
