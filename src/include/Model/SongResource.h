#pragma once

#include <QObject>
#include <QJSEngine>

class SongResource : public QObject
{
    Q_OBJECT;
public:
    explicit SongResource(QObject *parent=nullptr);
    void fetchSong(const QString &songId);

private:
    QJSEngine* jsEngine;

    // 工具方法

    QString readFileToString(const QString &filePath);
};
