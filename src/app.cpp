#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "app.h"
#include "MainWin.h"
#include "SearchPage.h"
#include "EventBus.h"
#include "GlobalObject.h"
#include "NodeProcessManager.h"
#include "Model.h"

int app_main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);        // 创建应用程序实例
    NodeProcessManager* nodeManager = new NodeProcessManager();
    GlobalObject::nodePrMan = nodeManager;

    Model model(&app);                             // 创建模型实例

    // 注册资源
    qmlRegisterType<MainWin>("ViewModel", 1, 0, "MainWinVM");
    qmlRegisterType<SearchPage>("ViewModel", 1, 0, "SearchPageVM");
    // 注册为 QML 单例
    qmlRegisterSingletonInstance("EventBus", 1, 0, "EventBus", EventBus::instance());

    QQmlApplicationEngine engine;

    engine.load(QUrl("qrc:/View/src/View/View.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}