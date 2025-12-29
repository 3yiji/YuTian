#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "app.h"
#include "MainWin.h"
#include "SearchPage.h"
#include "EventBus.h"

int app_main(int argc, char *argv[])
{
    // 注册资源
    qmlRegisterType<MainWin>("ViewModel", 1, 0, "MainWinVM");
    qmlRegisterType<SearchPage>("ViewModel", 1, 0, "SearchPageVM");
    // 注册为 QML 单例
    qmlRegisterSingletonInstance("EventBus", 1, 0, "EventBus", EventBus::instance());

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // ViewModel vm;
    // engine.rootContext()->setContextProperty("viewModel", &vm);

    engine.load(QUrl("qrc:/View/src/View/MainWin.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}