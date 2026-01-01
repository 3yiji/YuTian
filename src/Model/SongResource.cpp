#include "SongResource.h"
#include <QJSEngine>
#include "LXSingleBridge.h"
#include <QFile>

SongResource::SongResource(QObject *parent): QObject(parent) {
    QJSValue errorValue;
    QString jsCode;
    jsEngine = new QJSEngine(this);                     // 初始化 JavaScript 引擎
    jsEngine->installExtensions(jsEngine->AllExtensions);
    LXSingleBridge* bridge = new LXSingleBridge(jsEngine, this); // 创建桥接对象
    jsEngine->globalObject().setProperty(              // 将桥接对象暴露给 JS 环境
        "lx",
        jsEngine->newQObject(bridge)
    );

    qDebug() << QFile::exists(":/music_source/lx-music-source.js");
    qDebug() << QFile::exists("qrc:/music_source/lx-music-source.js");
    errorValue = jsEngine->evaluate(R"(
        globalThis = {lx};
    )");
    if (errorValue.isError()) {
        qDebug() << "1Uncaught exception at line"
                 << errorValue.property("lineNumber").toInt()
                 << ":" << errorValue.toString();
    }
    jsCode = readFileToString(":/music_source/lx-music-source.js");
    errorValue = jsEngine->evaluate(jsCode);
    if (errorValue.isError()) {
        qDebug() << "2Uncaught exception at line"
                 << errorValue.property("lineNumber").toInt()
                 << ":" << errorValue.toString();
    }

    jsCode = readFileToString(":/JsPro/api_request.js");
    errorValue = jsEngine->evaluate(jsCode);
    if (errorValue.isError()) {
        qDebug() << "3Uncaught exception at line"
                 << errorValue.property("lineNumber").toInt()
                 << ":" << errorValue.toString();
    }
}

void SongResource::fetchSong(const QString &songId)
{
    // Implementation for fetching song details by songId
}

QString SongResource::readFileToString(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open:" << path;
        return {};
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);  // Qt6
    // Qt5 用：in.setCodec("UTF-8");

    return in.readAll();
}
