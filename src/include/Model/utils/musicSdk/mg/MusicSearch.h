#pragma once

#include <QObject> 

namespace musicSdk::mg{
class MusicSearch : public QObject{
    Q_OBJECT
public:
    explicit MusicSearch(QObject *parent = nullptr){};
};
}