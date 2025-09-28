#include <QtTest>
#include "SaveLoadManager.hpp"
#include "turtlecontrol.h"

class test_SaveLoadManager : public QObject
{
    Q_OBJECT

public:
    test_SaveLoadManager();
    ~test_SaveLoadManager();

private slots:
    void test_saveLoadState();

private:
    void createDummyFile(const QString &filePath, const QString &content);
};

test_SaveLoadManager::test_SaveLoadManager() {}

test_SaveLoadManager::~test_SaveLoadManager() {}

void test_SaveLoadManager::test_saveLoadState()
{
    // Creating the manager and the turtle control
    SaveLoadManager manager;
    TurtleControl turtleControl;
    manager.setTurtleControl(&turtleControl);
    QString folderPath = "test_build_folder";

    // Ensure the folder exists
    QDir dir(folderPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    manager.setBuildFolder(folderPath);

    // Setting some state to the TurtleControl
    QPointF position(50, 50);
    qreal rotation = 45;
    bool penDown = true;
    float penRadius = 2;
    QColor penColor("#ff0000"); // red color

    turtleControl.set_position(position);
    turtleControl.set_rotation(rotation);
    turtleControl.set_pen_down(penDown);
    turtleControl.set_pen_radius(penRadius);
    turtleControl.set_pen_color(penColor);

    // Saving the state
    QString stateFileName = "test_state";
    manager.saveState(stateFileName);

    // Resetting the state to ensure it loads correctly
    turtleControl.set_position(QPointF(0, 0));
    turtleControl.set_rotation(0);
    turtleControl.set_pen_down(false);
    turtleControl.set_pen_radius(1);
    turtleControl.set_pen_color(Qt::black);

    // Loading the state
    manager.loadState(dir.filePath(stateFileName + ".txt"));

    // Verifying the loaded state
    QCOMPARE(turtleControl.position(), position);
    QCOMPARE(turtleControl.rotation(), rotation);
    QCOMPARE(turtleControl.pen_down(), penDown);
    QCOMPARE(turtleControl.pen_radius(), penRadius);
    QCOMPARE(turtleControl.pen_color(), penColor);

    // Clean up the saved state file
    QFile::remove(dir.filePath(stateFileName + ".txt"));
    dir.rmdir(folderPath);
}

QTEST_APPLESS_MAIN(test_SaveLoadManager)

#include "tst_testsaveloadmanager.moc"
