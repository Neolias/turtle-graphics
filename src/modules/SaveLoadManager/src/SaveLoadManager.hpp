#ifndef SAVELOADMANAGER_HPP
#define SAVELOADMANAGER_HPP

#include <QObject>
#include <QString>
#include <QPointF>
#include <QColor>
#include <QDateTime>
#include <QFile>
#include <QImage>
#include <QQuickWindow>
#include <QTextStream>
#include <QDir>
#include <QUrl>

// Forward declaration of TurtleControl and CLI classes
class TurtleControl;
class CLI;

/**
 * @class SaveLoadManager
 * @brief Manages saving and loading of application state, screenshots, and related data.
 *
 * This class provides functions to save screenshots, application state, and load states.
 * It exposes relevant properties and functions to QML.
 */
class SaveLoadManager : public QObject
{
    Q_OBJECT

    /**
     * @brief The main window reference.
     *
     * This property holds a reference to the main window object and is accessible from QML.
     */
    Q_PROPERTY(QObject* mainWindow READ mainWindow WRITE setMainWindow NOTIFY mainWindowChanged)

    /**
     * @brief The build folder path.
     *
     * This property holds the path to the folder used for saving and loading operations.
     * It is accessible from QML.
     */
    Q_PROPERTY(QString buildFolder READ buildFolder WRITE setBuildFolder NOTIFY buildFolderChanged)

public:
    /**
     * @brief Constructs a SaveLoadManager object.
     * @param parent The parent QObject. Defaults to nullptr.
     */
    explicit SaveLoadManager(QObject *parent = nullptr);

    /**
     * @brief Gets the main window reference.
     * @return A pointer to the main window object.
     */
    QObject* mainWindow() const;

    /**
     * @brief Gets the build folder path.
     * @return The build folder path as a QString.
     */
    QString buildFolder() const;

    /**
     * @brief Sets the main window reference.
     * @param mainWindow A pointer to the main window object.
     * 
     * This function is Q_INVOKABLE to allow QML access.
     */
    Q_INVOKABLE void setMainWindow(QObject *mainWindow);

    /**
     * @brief Sets the TurtleControl reference.
     * @param turtleControl A pointer to the TurtleControl object.
     * 
     * This function is Q_INVOKABLE to allow QML access.
     */
    Q_INVOKABLE void setTurtleControl(TurtleControl *turtleControl);

    /**
     * @brief Sets the CLI reference.
     * @param cli A pointer to the CLI object.
     * 
     * This function is Q_INVOKABLE to allow QML access.
     */
    Q_INVOKABLE void setCLI(CLI *cli);

    /**
     * @brief Sets the build folder path.
     * @param buildFolder The new build folder path as a QString.
     * 
     * This function is Q_INVOKABLE to allow QML access.
     */
    Q_INVOKABLE void setBuildFolder(const QString &buildFolder);

    /**
     * @brief Saves a screenshot to the specified file.
     * @param none.
     *
     * This function is Q_INVOKABLE to allow QML access.
     */
    Q_INVOKABLE void saveScreenshot();

    /**
     * @brief Saves the current state to the specified file.
     * @param fileName The name of the file to save the state to.
     *
     * This function is Q_INVOKABLE to allow QML access.
     */
    Q_INVOKABLE void saveState(const QString &fileName);

    /**
     * @brief Loads a state from the specified file.
     * @param filePath The path of the file to load the state from.
     *
     * This function is Q_INVOKABLE to allow QML access.
     */
    Q_INVOKABLE void loadState(const QString &filePath);

signals:
    /**
     * @brief Signal emitted when the main window reference changes.
     */
    void mainWindowChanged();

    /**
     * @brief Signal emitted when the build folder path changes.
     */
    void buildFolderChanged();

private:
    /**
     * @brief Saves content to a specified file.
     * @param filePath The path of the file to save content to.
     * @param content The content to save as a QString.
     */
    void saveToFile(const QString &filePath, const QString &content);

    /**
     * @brief Loads content from a specified file.
     * @param filePath The path of the file to load content from.
     * @return The loaded content as a QStringList.
     */
    QStringList loadFromFile(const QString &filePath);

    /**
     * @brief Gets the current date and time in the format "dd_MM_hh_mm".
     * @return The current date and time as a formatted QString.
     */
    QString getCurrentDateTimeString() const;

    /**
     * @brief Logs and emits output messages to the CLI.
     * @param message The message to log and emit.
     */
    void logAndEmitOutput(const QString &message);

private:
    QObject *m_mainWindow;         ///< Main window reference.
    TurtleControl *m_turtleControl; ///< TurtleControl reference.
    CLI *m_cli;                    ///< CLI reference.
    QString m_buildFolder;         ///< Folder path for saving/loading.
};

#endif // SAVELOADMANAGER_HPP

