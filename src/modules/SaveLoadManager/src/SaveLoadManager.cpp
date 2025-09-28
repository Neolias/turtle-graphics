#include "SaveLoadManager.hpp"
#include "turtlecontrol.h"
#include "CLI.hpp"

SaveLoadManager::SaveLoadManager(QObject *parent)
    : QObject(parent),
    m_mainWindow(nullptr),
    m_turtleControl(nullptr),
    m_cli(nullptr),
    m_buildFolder("") {}

QObject* SaveLoadManager::mainWindow() const {
    return m_mainWindow;
}

void SaveLoadManager::setMainWindow(QObject *mainWindow) {
    if (m_mainWindow != mainWindow) {
        m_mainWindow = mainWindow;
        emit mainWindowChanged();
    }
}

void SaveLoadManager::setTurtleControl(TurtleControl *turtleControl) {
    if (m_turtleControl != turtleControl) {
        m_turtleControl = turtleControl;
    }
}

void SaveLoadManager::setCLI(CLI *cli) {
    if (m_cli != cli) {
        m_cli = cli;
    }
}

QString SaveLoadManager::buildFolder() const {
    return m_buildFolder;
}

void SaveLoadManager::setBuildFolder(const QString &buildFolder) {
    if (m_buildFolder != buildFolder) {
        m_buildFolder = buildFolder;
        emit buildFolderChanged();
    }
}

void SaveLoadManager::saveScreenshot() {
    if (!m_mainWindow) {
        return;
    }

    auto quickWindow = qobject_cast<QQuickWindow*>(m_mainWindow);
    if (!quickWindow) {
        return;
    }

    QImage image = quickWindow->grabWindow();
    QDir dir(m_buildFolder);
    if (!dir.exists()) {
        return;
    }

    QString fileName = QString("Screenshot_%1.png").arg(getCurrentDateTimeString());
    QString filePath = dir.filePath(fileName);
    
    if (image.save(filePath)) {
        logAndEmitOutput("Screenshot saved successfully: " + filePath);
    } else {
        logAndEmitOutput("Failed to save screenshot: " + filePath);
    }
}

void SaveLoadManager::saveState(const QString &fileName)
{
    if (!m_turtleControl) {
        return;
    }

    // Get the turtle's current state: position, rotation, pen settings
    QString state = QString("%1;%2;%3;%4;%5;%6;")
        .arg(m_turtleControl->position().x())   // Turtle position x
        .arg(m_turtleControl->position().y())   // Turtle position y
        .arg(m_turtleControl->rotation())       // Turtle rotation
        .arg(m_turtleControl->pen_down())       // Pen state (down or up)
        .arg(m_turtleControl->pen_radius())     // Pen radius
        .arg(m_turtleControl->pen_color().name()); // Pen color as hex string

    // Save lines data
    QVector<Line> lines = m_turtleControl->get_lines();  // Get all the lines
    QString lineData;
    for (const auto &line : lines) {
        lineData += QString("%1;%2;%3;%4;%5;%6\n")
                        .arg(line.start_.x())     // Line start x
                        .arg(line.start_.y())     // Line start y
                        .arg(line.end_.x())       // Line end x
                        .arg(line.end_.y())       // Line end y
                        .arg(line.color_.name())  // Line color in hex
                        .arg(line.width_);        // Line width
    }

    // Combine turtle data and line data
    QString fullData = state + "\n" + lineData;

    // Save the full state to a file
    QDir dir(m_buildFolder);
    if (!dir.exists()) {
        return;
    }

    QString filePath = dir.filePath(fileName + ".txt");
    saveToFile(filePath, fullData);
    logAndEmitOutput("State saved successfully: " + filePath);
}

void SaveLoadManager::loadState(const QString& filePath) {
    if (!m_turtleControl) {
        return;
    }

    // Load content from the file
    QStringList stateList = loadFromFile(filePath);

    // Check if the list is empty
    if (stateList.isEmpty()) {
        logAndEmitOutput("Failed to load state: File is empty or does not exist.");
        return;  // No data to process
    }

    // Load turtle data from the first line
    QStringList state = stateList[0].split(";");
    if (state.size() < 6) {
        logAndEmitOutput("Failed to load state: Invalid data format.");
        return;  // Invalid data, return early
    }

    // Parse the turtle state
    double posX = state[0].toDouble();
    double posY = state[1].toDouble();
    double rotation = state[2].toFloat();
    bool penDown = (state[3].toInt() != 0);  // Convert pen down state (0 or 1)
    float penRadius = state[4].toFloat();
    QColor penColor(state[5]);

    QPointF position(posX, posY);
    m_turtleControl->set_position(position);
    m_turtleControl->set_rotation(rotation);
    m_turtleControl->set_pen_down(penDown);
    m_turtleControl->set_pen_radius(penRadius);
    m_turtleControl->set_pen_color(penColor);

    // Load line data from the subsequent lines
    QVector<Line> lines;
    for (int i = 1; i < stateList.size(); ++i) {
        QString lineData = stateList[i];

        // Split by semicolons (since each line is separated by ; in your case)
        QStringList lineParts = lineData.split(";", Qt::SkipEmptyParts);

        if (lineParts.size() != 5 && lineParts.size() != 6) {
            continue;  // Invalid line format, skip it
        }

        // Parse start and end points
        QPointF start(lineParts[0].toDouble(), lineParts[1].toDouble());
        QPointF end(lineParts[2].toDouble(), lineParts[3].toDouble());

        // Parse color (hex string)
        QColor color(lineParts[4]);

        // Parse width
        float width = lineParts[5].toFloat();

        // Create Line object and add it to the lines vector
        lines.append(Line(start, end, color, width));
    }

    // Set the loaded lines to TurtleControl
    m_turtleControl->set_lines(lines);
    logAndEmitOutput("State loaded successfully: " + filePath);
}

void SaveLoadManager::saveToFile(const QString &filePath, const QString &content) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logAndEmitOutput("Failed to save to file: " + filePath);
        return;
    }

    QTextStream out(&file);
    out << content;
    file.close();
}

QStringList SaveLoadManager::loadFromFile(const QString &filePath)
{
    QString localFilePath = filePath;
    if (filePath.startsWith("file://")) {
        localFilePath = QUrl(filePath).toLocalFile();
    }

    QFile file(localFilePath);

    if (!file.exists()) {
        logAndEmitOutput("File does not exist: " + localFilePath);
        return QStringList();  // Return an empty list if the file doesn't exist
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logAndEmitOutput("Failed to open file: " + localFilePath);
        return QStringList();  // Return an empty list if file cannot be opened
    }

    QTextStream in(&file);
    QStringList lines;

    // Read each line from the file and append it to the QStringList
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lines.append(line);  // Add the trimmed line to the list
    }

    file.close();
    return lines;  // Return the list of lines
}

QString SaveLoadManager::getCurrentDateTimeString() const {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("dd_MM_hh_mm");
    return formattedDateTime;
}

void SaveLoadManager::logAndEmitOutput(const QString &message) {
    if (m_cli) {
        m_cli->appendToOutputLog(message);
        emit m_cli->outputChanged();
        emit m_cli->commandProcessed(message);
    }
}

