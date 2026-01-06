#pragma once
#include <QObject>
#include "NodeProcessManager.h"
#include <QFileInfo>
#include <QHash>

class SourceControl : public QObject
{
    Q_OBJECT
public:
    explicit SourceControl(NodeProcessManager* nodeProcessManager, QObject *parent = nullptr);

    void initSource(QFileInfo scriptPath);

    struct SourceInfo{
        QString name;
        QString type;
        QList<QString> actions;
        QList<QString> qualitys;
    };

    QHash<QString, SourceInfo> getSourceInfoMap(){
        return sourceInfoMap;
    }

    void test(){
        connect(this, &SourceControl::initSourceFinished, this,
                [](bool success){
            if(success){
                qDebug() << "Source initialized successfully.";
                qDebug() << "Source initialized successfully.";
            } else {
                qDebug() << "Failed to initialize source.";
            }
        });

        
    }

signals:    
    void initSourceFinished(bool success);


private:
    NodeProcessManager* nodeProcessManager;
    bool initSourceResult = false;
    QHash<QString, SourceInfo> sourceInfoMap;
};