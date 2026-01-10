#include "SourceControl.h"
#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include "NodeProcessManager.h"
#include <QFileInfo>
#include <QFuture>
#include "YuTian.h"

SourceControl::SourceControl(NodeProcessManager* nodeProcessManager, QObject *parent)
    : QObject(parent)
{
    this->nodeProcessManager = nodeProcessManager;
    QFileInfo scriptPath = QFileInfo("./src/api_lx_source/lx-music-source.js");
    // this->initSource(scriptPath);
}

QFuture<QMap<QString, SourceControl::SourceInfo>> SourceControl::getSourceInfoMap(){
    auto promise = std::make_shared<QPromise<QMap<QString, SourceInfo>>>();
    promise->start();

    if(!sourceInfoMap.isEmpty()&& initSourceResult){   // 存在值，且初始化成功
        promise->addResult(this->sourceInfoMap);
        promise->finish();
        return promise->future();
    }

    if(hasInited){              // 已经初始化过，但失败了
        promise->setException(std::make_exception_ptr(std::runtime_error("Source initialization failed")));
        promise->finish();
        return promise->future();
    }

    if(sourcePaThList.isEmpty()){               // 没有源，直接报错
        promise->setException(std::make_exception_ptr(std::runtime_error("No source scripts available")));
        promise->finish();
        return promise->future();
    }
    this->initSource(QFileInfo(sourcePaThList.first()));
    connect(this, &SourceControl::initSourceFinished, this,
            [this, promise](bool success){
        if(!success){
            promise->setException(std::make_exception_ptr(std::runtime_error("Failed to initialize source scripts")));
            promise->finish();
            return;
        }
        promise->addResult(this->sourceInfoMap);
        promise->finish();
    });
    return promise->future();
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
            hasInited = true;             // 已经初始化过
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
            QMap<QString, SourceInfo> sourceInfoMap_temp;
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
                sourceInfoMap_temp.insert(key, sourceInfo); 
            }
            if(sourceInfoMap_temp.isEmpty()){
                initSourceResult = false;
                emit initSourceFinished(false);
                return;
            }
            sourceInfoMap = sourceInfoMap_temp;
            initSourceResult = true;
            emit initSourceFinished(true);
            
        });

}
