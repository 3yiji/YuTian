// #include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "CounterModel.h"
#include "CounterViewModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    CounterModel model;
    CounterViewModel viewModel(&model);
    
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("viewModel", &viewModel);
    engine.load(QUrl(QStringLiteral("qrc:/CounterView.qml")));
    
    return app.exec();
} 