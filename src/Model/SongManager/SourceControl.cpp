#include "SourceControl.h"
#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include "NodeProcessManager.h"
#include <QFileInfo>

SourceControl::SourceControl(NodeProcessManager* nodeProcessManager, QObject *parent)
    : QObject(parent)
{
    this->nodeProcessManager = nodeProcessManager;
    QFileInfo scriptPath = QFileInfo("./src/api_lx_source/lx-music-source.js");
    this->initSource(scriptPath);
}

void SourceControl::initSource(QFileInfo scriptPath){
    // Implemenvoid NodeProcessManager::InitSource(QUrl scriptPath){
    int timeout = 5000; // 5秒超时
    QFile file(scriptPath.filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open:" << scriptPath.filePath();
        emit initSourceFinished(false);
        return;
    }
    QTextStream in(&file);
    QString scriptCode = in.readAll();         // 读到脚本的所有信息
    file.close();

    // 准备发送给 Node.js 进程的请求数据
     
    // 1. 构造并发送请求
    QJsonObject msg;
    msg["method"] = "initSource";
    msg["params"] = scriptCode;
    nodeProcessManager->request("initSource", msg, timeout,
        [this](QString id, QJsonObject response, bool success, QString errorMsg){
            if(!success){               // 请求失败 
                initSourceResult = false;
                emit initSourceFinished(false);
                return;
            }
            bool successResult = response["success"].toBool();
            if(!successResult){                         // node运行失败
                initSourceResult = false;
                emit initSourceFinished(false);
            }
            QJsonObject data = response["data"].toObject();

            if(!data.value("status").toBool()){         // 初始化失败
                initSourceResult = false;
                emit initSourceFinished(false); 
                return;
            }
            QJsonObject sources = data["sources"].toObject();
            for(QJsonObject::iterator it = sources.begin(); it != sources.end(); ++it) {
                QString key = it.key();
                QJsonObject value = it.value().toObject();
                SourceInfo sourceInfo;
                sourceInfo.name = value["name"].toString();
                sourceInfo.type = value["type"].toString();
                // Convert QJsonArray to QList<QString> for actions
                QJsonArray actionsArray = value["actions"].toArray();
                for(const QJsonValue &val : actionsArray) {
                    sourceInfo.actions.append(val.toString());
                }
                // Convert QJsonArray to QList<QString> for qualitys
                QJsonArray qualitysArray = value["qualitys"].toArray();
                for(const QJsonValue &val : qualitysArray) {
                    sourceInfo.qualitys.append(val.toString());
                }
                sourceInfoMap.insert(key, sourceInfo); 
            }
            initSourceResult = true;
            emit initSourceFinished(true);
            
        });

}
