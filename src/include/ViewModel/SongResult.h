#pragma once
#include <QObject>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVariant>
#include <QVector>

class SongResult : public QObject {
    Q_OBJECT


    enum class Platform {
        KuWo,      // 酷我
        KuGou,     // 酷狗
        QQ,        // QQ音乐
        NetEase,   // 网易云
        MiGu,      // 咪咕
    };
    Q_ENUM(Platform)

    // origin_data[平台][歌单列表][属性键值对]
    QHash<Platform, QVector<QHash<QString, QString>>> origin_data;

    // 页面显示的数据
    QHash<Platform, QVector<QVector<QString>>> display_data;


public:
    explicit SongResult(QObject *parent = nullptr); 
};