#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "app.h"
#include "MainWin.h"
#include "SearchPage.h"
#include "EventBus.h"
#include "GlobalObject.h"
#include "QProcess"

int app_main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    QProcess* nodeProcess = new QProcess(&app);         // 启动 Node.js 进程
    GlobalObject::nodeProcess = nodeProcess;
    nodeProcess->start("node", {
        "node_runner.js"
    });
    if (!nodeProcess->waitForStarted()) {
        qFatal("Node start failed");
    }

    // 注册资源
    qmlRegisterType<MainWin>("ViewModel", 1, 0, "MainWinVM");
    qmlRegisterType<SearchPage>("ViewModel", 1, 0, "SearchPageVM");
    // 注册为 QML 单例
    qmlRegisterSingletonInstance("EventBus", 1, 0, "EventBus", EventBus::instance());


    
    QQmlApplicationEngine engine;

    engine.load(QUrl("qrc:/View/src/View/MainWin.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}