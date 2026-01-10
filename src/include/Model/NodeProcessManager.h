#pragma once
#include <QObject>
#include <QTimer>
#include <QJsonDocument>
#include <QHash>
#include <QProcess>
#include <QJsonObject>

class NodeProcessManager : public QObject
{
    Q_OBJECT;
public:
    explicit NodeProcessManager(QObject *parent = nullptr);

    // void require(RequestInfo requestInfo, QJsonObject requestData);
    void request(QString id, QJsonObject requestBody, int timeout, 
        std::function<void(QString, QJsonObject, bool, QString)> callback = nullptr);


    void test(){
        connect(this, &NodeProcessManager::responseReceived, this,
                [](QString requestId, QJsonObject responseBody, bool success, QString errorMsg){
            if(success){
                qDebug() << "Request ID:" << requestId << "Response Body:" << responseBody;
            } else {
                qDebug() << "Request ID:" << requestId << "Error:" << errorMsg;
            }
        });
    }

signals:
    void responseReceived(QString requestId, QJsonObject responseBody, bool success, QString errorMsg="");
    void initSourceFinished(bool success);

private slots:
    void onReadyReadOutput();

private:
    QString initSourceId = "initSource";

    struct RequestInfo{
        QString id;
        std::shared_ptr<QTimer> timer;
        std::function<void(QString, QJsonObject, bool, QString)> callback;
    };
    QHash<QString, RequestInfo> m_requestMap;
    QProcess* process;
    int m_requestIdCounter = 0;

    bool m_isProcessReady = false;

};
