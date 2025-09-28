#include "CLI.hpp"
#include "parser.hpp"


// Constructor: Initializes the CLI instance
CLI::CLI(QObject *parent)
    : QObject(parent), parser_(nullptr), historyIndex_(-1) {
    outputLog_.append("Welcome to Turtle graphics");
    emit outputChanged(); // Notify UI of the initial state
}

// Sets the Parser object used by the CLI
void CLI::setParser(Parser *parser) {
    parser_ = parser;
}

// Processes a command entered by the user
void CLI::processCommand(const QString &command) {
    QString trimmedCommand = command.trimmed();
    if (trimmedCommand.isEmpty()) {
        // Handle empty command case
        QString errorMessage = "Error: Command cannot be empty.";
        emit commandProcessed(errorMessage);
        outputLog_.append(errorMessage);
        emit outputChanged();
        return;
    }

    // Skip script loading logic here

    // Add the command to the history for undo/redo functionality
    commandHistory_.push_back(trimmedCommand);
    historyIndex_ = commandHistory_.size();

    // Handle special commands
    if (trimmedCommand == "clear") {
        clearOutput();
        return;
    } else if (trimmedCommand == "quit") {
        emit commandProcessed("Application quitting...");
        outputLog_.append("Application quitting...");
        emit requestQuit();
        emit outputChanged();
        return;
    }

    if (!parser_) {
        QString errorMessage = "Error: Parser not set.";
        emit commandProcessed(errorMessage);
        outputLog_.append(errorMessage);
        emit outputChanged();
        return;
    }

    try {
        // Parse and execute the command
        std::vector<std::string> parsedCommands = parser_->parse_line(trimmedCommand);
        for (const auto &cmd : parsedCommands) {
            commandHistory_.push_back(QString::fromStdString(cmd));
        }
    } catch (const std::exception &e) {
        // Handle parsing or execution errors
        QString errorMessage = QString("Error: %1").arg(e.what());
        emit commandProcessed(errorMessage);
        outputLog_.append(errorMessage);
        emit outputChanged();
        return;
    }

    // Log the successful processing of the command
    QString output = QString("Processed: %1").arg(trimmedCommand);
    outputLog_.append(output);
    if (outputLog_.size() > 100) {
        outputLog_.removeFirst(); // Maintain a max size for the log
    }

    emit commandProcessed(output);
    emit outputChanged();
}

// Retrieves the current output log
QString CLI::getOutput() const {
    return outputLog_.join("\n");
}

// Clears the output log
void CLI::clearOutput() {
    outputLog_.clear();
    emit commandProcessed("Output cleared.");
    emit outputChanged();
}

// Retrieves the command history as a QStringList
QStringList CLI::getCommandHistory() const {
    return QStringList(commandHistory_.begin(), commandHistory_.end());
}

// Assuming parser_ and outputLog_ are defined elsewhere in the CLI class
void CLI::loadScript(const QString &filename) {
    // Convert file URL to local file path
    QUrl fileUrl(filename);
    QString localFilePath = fileUrl.toLocalFile();
    
    // Debugging output: Print the local file path
    qDebug() << "Attempting to load script from file:" << localFilePath;

    // Check if the file exists
    if (!QFile::exists(localFilePath)) {
        QString message = "File does not exist: " + localFilePath;
        outputLog_.append(message);
        emit commandProcessed(message);
        emit outputChanged();
        return;
    }

    // Debugging output: Print the current working directory
    qDebug() << "Current working directory:" << QDir::currentPath();

    // Attempt to open the file for reading
    std::ifstream file(localFilePath.toStdString());
    if (!file.is_open()) {
        QString message = "Failed to open file: " + localFilePath;
        outputLog_.append(message);
        emit commandProcessed(message);
        emit outputChanged();
        return;
    }

    // Check if the file is empty
    file.seekg(0, std::ios::end);  // Move to the end to check size
    if (file.tellg() == 0) {
        QString message = "File is empty: " + localFilePath;
        outputLog_.append(message);
        emit commandProcessed(message);
        emit outputChanged();
        return;
    }

    file.seekg(0, std::ios::beg);  // Reset position for reading

    std::vector<std::string> parsedCommands = parser_->parse_script(file);
    for (const auto &cmd : parsedCommands) { // store the commands run by script to history
        commandHistory_.push_back(QString::fromStdString(cmd));
    }

    QString message = "File loaded successfully: " + localFilePath;
    outputLog_.append(message);
    emit commandProcessed(message);
    emit outputChanged();
}

void CLI::appendToOutputLog(const QString &message) {
    outputLog_.append(message);
    if (outputLog_.size() > 100) {
        outputLog_.removeFirst(); // Maintain a max size for the log
    }
    emit outputChanged();
}

