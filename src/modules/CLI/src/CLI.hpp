#ifndef CLI_HPP
#define CLI_HPP

#include <QObject>
#include <QString>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <vector>
#include <QDir>
#include <QUrl>
#include <fstream>


class Parser;  // Forward declaration of the Parser class

/**
 * @brief The CLI class provides an interface for processing commands, managing history,
 *        and interacting with a parser and state management.
 */
class CLI : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the CLI class.
     * @param parent Pointer to the parent QObject (optional).
     */
    explicit CLI(QObject *parent = nullptr);

    /**
     * @brief Sets the parser instance to process commands.
     * @param parser Pointer to the Parser object.
     * @note This function is callable from QML.
     */
    Q_INVOKABLE void setParser(Parser *parser);

    /**
     * @brief Processes a given command by parsing and executing it.
     * @param command The command string to process.
     * @note This function is callable from QML.
     */
    Q_INVOKABLE void processCommand(const QString &command);

    /**
     * @brief Retrieves the current output log as a single string.
     * @return A QString containing the concatenated output log.
     * @note This function is callable from QML.
     */
    Q_INVOKABLE QString getOutput() const;

    /**
     * @brief Clears the output log.
     * @note This function is callable from QML.
     */
    Q_INVOKABLE void clearOutput();

    /**
     * @brief Retrieves the current command history as a QStringList.
     * @return A QStringList containing all commands in the history.
     * @note This function is callable from QML.
     */
    Q_INVOKABLE QStringList getCommandHistory() const;
      
     /**
     * @brief Loads and processes a script file by passing its contents to the parser.
     *
     * This method opens the specified script file, reads its contents,
     * and passes an ifstream to the parser for processing.
     *
     * @param filename The filename of the script to be loaded.
     */
    Q_INVOKABLE void loadScript(const QString &filename);
    
     /**
     * @brief Appends a message to the output log and emits the appropriate signals.
     * @param message The message to append to the output log.
     */
    void appendToOutputLog(const QString &message); 

signals:
    /**
     * @brief Emitted when a command has been processed.
     * @param message A QString containing the processed command or result.
     */
    void commandProcessed(const QString &message);

    /**
     * @brief Emitted when the output log changes.
     */
    void outputChanged();

    /**
     * @brief Emitted to request the application to quit.
     */
    void requestQuit();
    
private:
    Parser *parser_;                ///< Pointer to the Parser instance used to process commands.
    QList<QString> commandHistory_; ///< List of commands entered by the user, for undo/redo functionality.
    int historyIndex_;              ///< Index to track the current position in the command history.
    QStringList outputLog_;         ///< List storing log messages for the output.
};

#endif // CLI_HPP

