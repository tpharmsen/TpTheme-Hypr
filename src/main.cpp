#include "WidgetBackend.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QRegularExpression>
#include <QUrl>
#include <QtQml>
#include <QDebug>
#include "SvgClockItem.h"

namespace {
bool parsePair(const QString &value, const QRegularExpression &separator,
               int *first, int *second)
{
    const QStringList parts = value.split(separator, Qt::KeepEmptyParts);
    if (parts.size() != 2)
        return false;

    bool firstOk = false;
    bool secondOk = false;
    const int parsedFirst = parts.at(0).toInt(&firstOk);
    const int parsedSecond = parts.at(1).toInt(&secondOk);
    if (!firstOk || !secondOk)
        return false;

    *first = parsedFirst;
    *second = parsedSecond;
    return true;
}
}

int main(int argc, char **argv)
{
    if (qEnvironmentVariableIsEmpty("QT_WAYLAND_SHELL_INTEGRATION"))
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", "layer-shell");

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("tp-theme-hypr"));
    app.setApplicationDisplayName(QStringLiteral("TpTheme Hypr widgets"));

    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption widgetOption(QStringLiteral("widget"),
                                    QStringLiteral("Widget to display (panel, clock, system, quote, journal, animation)."),
                                    QStringLiteral("name"), QStringLiteral("clock"));
    QCommandLineOption positionOption(
        QStringLiteral("position"),
        QStringLiteral("Position on the screen as X,Y (top-left origin)."),
        QStringLiteral("x,y"));
    QCommandLineOption sizeOption(
        QStringLiteral("size"),
        QStringLiteral("Size as WIDTHxHEIGHT."),
        QStringLiteral("widthxheight"));
    parser.addOption(widgetOption);
    parser.addOption(positionOption);
    parser.addOption(sizeOption);
    parser.process(app);

    int positionX = 0;
    int positionY = 0;
    const bool hasPosition = parser.isSet(positionOption);
    if (hasPosition
        && (!parsePair(parser.value(positionOption),
                       QRegularExpression(QStringLiteral("[,xX]")),
                       &positionX, &positionY)
            || positionX < 0 || positionY < 0)) {
        qCritical().noquote() << "--position must be non-negative X,Y coordinates.";
        return 2;
    }

    int width = 0;
    int height = 0;
    const bool hasSize = parser.isSet(sizeOption);
    if (hasSize
        && (!parsePair(parser.value(sizeOption),
                       QRegularExpression(QStringLiteral("[xX,]")),
                       &width, &height)
            || width <= 0 || height <= 0)) {
        qCritical().noquote() << "--size must be positive WIDTHxHEIGHT.";
        return 2;
    }

    const QStringList widgets{QStringLiteral("panel"), QStringLiteral("clock"), QStringLiteral("system"),
                              QStringLiteral("quote"), QStringLiteral("journal"),
                              QStringLiteral("animation")};
    const QString widget = widgets.contains(parser.value(widgetOption))
        ? parser.value(widgetOption)
        : QStringLiteral("clock");

    WidgetBackend backend;
    qmlRegisterType<SvgClockItem>("TpTheme.Hypr", 1, 0, "SvgClock");
    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::warnings,
                     [](const QList<QQmlError> &warnings) {
                         for (const QQmlError &warning : warnings)
                             qWarning().noquote() << warning.toString();
                     });
    engine.rootContext()->setContextProperty(QStringLiteral("backend"), &backend);
    engine.rootContext()->setContextProperty(QStringLiteral("commandLinePositionSet"), hasPosition);
    engine.rootContext()->setContextProperty(QStringLiteral("commandLinePositionX"), positionX);
    engine.rootContext()->setContextProperty(QStringLiteral("commandLinePositionY"), positionY);
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/TpTheme/Hypr/qml/%1.qml").arg(
        widget.left(1).toUpper() + widget.mid(1))));

    if (engine.rootObjects().isEmpty())
        return 1;
    if (hasSize) {
        QObject *window = engine.rootObjects().constFirst();
        window->setProperty("width", width);
        window->setProperty("height", height);
    }
    return app.exec();
}
