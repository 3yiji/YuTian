#include "SongManager.h"
#include <QUrl>

SongManager::SongManager(QObject *parent)
    : QObject(parent)
{
}

QUrl SongManager::getSongUrl(const SongInfo &songInfo){
    return QUrl();
}

