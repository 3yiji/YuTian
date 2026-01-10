#include "YuTian.h"
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QObject>

#include "KuWoSearch.h"
#include "KuGouSearch.h"
#include "QQSearch.h"
#include "SearchSong.h"
#include "NetEaseSearch.h"
#include "MiGuSearch.h"
#include "Model.h"
#include "NodeProcessManager.h"
#include "SourceControl.h"
#include "MusicUrl.h"
#include "SongManager.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    NodeProcessManager nodeManager;
    SongManager songManager(&nodeManager);
    songManager.test();

    return app.exec();
}

