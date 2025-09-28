#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "modules/Parser/src/parser.hpp"
#include "modules/CLI/src/CLI.hpp"
#include "modules/SaveLoadManager/src/SaveLoadManager.hpp"
#include "modules/Canvas/src/canvas.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Parser>("ParserModule", 1, 0, "Parser");
    qmlRegisterType<CLI>("CLIModule", 1, 0, "CLI");
    qmlRegisterType<SaveLoadManager>("SaveLoadManagerModule", 1, 0, "SaveLoadManager");
    qmlRegisterType<Canvas>("CanvasModule", 1, 0, "Canvas");

    QQmlApplicationEngine engine;
    engine.addImportPath(QString("%1/src/modules").arg(QGuiApplication::applicationDirPath()));

    engine.loadFromModule("turtlegraphics", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
