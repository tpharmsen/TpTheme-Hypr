#include "WidgetBackend.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QUrl>
#include <QtQml>
#include <QDebug>
#include "SvgClockItem.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("tp-theme-hypr"));
    app.setApplicationDisplayName(QStringLiteral("TpTheme Hypr widgets"));

    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption widgetOption(QStringLiteral("widget"),
                                    QStringLiteral("Widget to display (clock, system, quote, journal, animation)."),
                                    QStringLiteral("name"), QStringLiteral("clock"));
    parser.addOption(widgetOption);
    parser.process(app);

    const QStringList widgets{QStringLiteral("clock"), QStringLiteral("system"),
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
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/TpTheme/Hypr/qml/%1.qml").arg(
        widget.left(1).toUpper() + widget.mid(1))));

    if (engine.rootObjects().isEmpty())
        return 1;
    return app.exec();
}
