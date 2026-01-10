#include "NodeProcessManager.h"
#include <QTimer>
#include <QJsonDocument>
#include <QProcess>
#include <QJsonObject>
#include <QFile>
#include <QTimer>

NodeProcessManager::NodeProcessManager(QObject *parent)
    : QObject(parent)
{
    process = new QProcess(this);         // 启动 Node.js 进程
    connect(process, &QProcess::readyReadStandardError, this, [this]() {
        qDebug() << "Node.js stderr:" << process->readAllStandardError();
    });
    process->start("node", {
        "--inspect=9229",          // node命令的调试参数（先写）
        "src/api_lx_source/node_runner.js"  // 脚本文件路径（后写）
    });
    if (!process->waitForStarted()) {
        qFatal("Node start failed");
    }
    qDebug() << "Node.js process started with PID2:" << process->processId();
    connect(process, &QProcess::readyReadStandardOutput, this, &NodeProcessManager::onReadyReadOutput);
}   

void NodeProcessManager::request(QString id, QJsonObject requestBody, int timeout, std::function<void((QString, QJsonObject, bool, QString))> callback){
    // 生成唯一请求ID
    QString requestId = QString::number(++m_requestIdCounter);
    
    QJsonObject requestObj;
    requestObj["id"] = requestId;
    requestObj["body"] = requestBody;
    QJsonDocument doc(requestObj);
    qDebug().noquote() << "Send JSON:" << QJsonDocument(doc).toJson(QJsonDocument::Indented);
    QByteArray sendData = doc.toJson(QJsonDocument::Compact) + '\n';
    
    // 2. 创建超时定时器
    auto timeoutTimer = std::make_shared<QTimer>(this);
    timeoutTimer->setSingleShot(true);  
    timeoutTimer->setInterval(timeout);

    // 3. 存储请求信息（ID、定时器、回调）
    RequestInfo info;
    info.timer = timeoutTimer;
    info.id = id;
    info.callback = callback;
    m_requestMap[requestId] = info;

    // 4. 绑定超时信号
    connect(timeoutTimer.get(), &QTimer::timeout, this, [this, requestId]() {
        if (m_requestMap.contains(requestId)) {
            RequestInfo requestInfo = m_requestMap[requestId];
            emit responseReceived(requestInfo.id, QJsonObject(), false, "请求超时");
            if(requestInfo.callback){                           // 调用回调函数
                requestInfo.callback(requestInfo.id, QJsonObject(), false, "请求超时");
            }
            qDebug() << "Request ID:" << requestId << "timed out.";
            m_requestMap.remove(requestId);                 // 移除请求记录
        }
    }, Qt::SingleShotConnection);

    // 启动定时器
    timeoutTimer->start();
        
    // process->write(sendData);                  // 写入数据
    // process->waitForBytesWritten(30000);      // 最多等待30秒写入完成
    qint64 bytesWritten = process->write(sendData);
    if (bytesWritten == -1) {
        qWarning() << "Failed to write to Node.js process";
        return;
    }
}

void NodeProcessManager::onReadyReadOutput(){                   // 读取 Node.js 输出
    QByteArray data = process->readAllStandardOutput();
    QString strData = QString::fromUtf8(data);
    QStringList lines = strData.split('\n', Qt::SkipEmptyParts);

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines[i];
        QJsonParseError parseErr;
        QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &parseErr);
        if (parseErr.error != QJsonParseError::NoError) {
            continue;
        }

        QJsonObject obj = doc.object();
        qDebug().noquote() << "Received JSON:" << QJsonDocument(obj).toJson(QJsonDocument::Indented);
        QString requestId = obj.value("id").toString();
        QJsonObject responseBody = obj.value("responseBody").toObject();
        if (m_requestMap.contains(requestId)) {                     // 匹配请求ID
            RequestInfo requestInfo = m_requestMap[requestId];
            emit responseReceived(requestInfo.id, responseBody, true, "");
            if(requestInfo.callback){                           // 调用回调函数
                requestInfo.callback(requestInfo.id, responseBody, true, "");
            }
            m_requestMap.remove(requestId);                 // 移除请求记录
            
        }
        else if(requestId == "Global_Message"){
            if(responseBody.value("msg").toString() == "Node.js process is ready"){
                m_isProcessReady = true;
            }
            else{
                emit responseReceived(requestId, responseBody, true, "");
            }
        }
    }
}

