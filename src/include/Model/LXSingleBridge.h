#include <QObject>
#include <QJSValue>
#include <QDebug>
#include <QHash>
#include "LXNetRequest.h"

class CurrentScriptInfo : public QObject {
    Q_OBJECT;
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged);
    Q_PROPERTY(QString version READ getVersion WRITE setVersion NOTIFY versionChanged);
    Q_PROPERTY(QString author READ getAuthor WRITE setAuthor NOTIFY authorChanged);
    Q_PROPERTY(QString homepage READ getHomepage WRITE setHomepage NOTIFY homepageChanged);
    Q_PROPERTY(QString rawScript READ getRawScript WRITE setRawScript NOTIFY rawScriptChanged);

public:
    explicit CurrentScriptInfo(QObject *parent = nullptr) : QObject(parent) {};

    QString getName() const { return m_name; }
    void setName(const QString &name) {
        if (m_name != name) {
            m_name = name;
            emit nameChanged();
        }
    }

    QString getDescription() const { return m_description; }
    void setDescription(const QString &description) {
        if (m_description != description) {
            m_description = description;
            emit descriptionChanged();
        }
    }

    QString getAuthor() const { return m_author; }
    void setAuthor(const QString &author) {
        if (m_author != author) {
            m_author = author;
            emit authorChanged();
        }
    }

    QString getVersion() const { return m_version; }
    void setVersion(const QString &version) {
        if (m_version != version) {
            m_version = version;
            emit versionChanged();
        }
    }

    QString getHomepage() const { return m_homepage; }
    void setHomepage(const QString &homepage) {
        if (m_homepage != homepage) {
            m_homepage = homepage;
            emit homepageChanged();
        }
    }

    QString getRawScript() const { return m_rawScript; }
    void setRawScript(const QString &rawScript) {
        if (m_rawScript != rawScript) {
            m_rawScript = rawScript;
            emit rawScriptChanged();
        }
    }

signals:
    void nameChanged();
    void descriptionChanged();
    void versionChanged();
    void authorChanged();
    void homepageChanged();
    void rawScriptChanged();

private:
    QString m_name = "t_name";
    QString m_description = "t_description";
    QString m_version = "0.1";
    QString m_author = "t_author";
    QString m_homepage = "t_homepage";
    QString m_rawScript = "t_rawScript";

};

class C_EVENT_NAMES : public QObject{
    Q_OBJECT;
    Q_PROPERTY(QString inited READ getInited CONSTANT);
    Q_PROPERTY(QString request READ getRequest CONSTANT);
    Q_PROPERTY(QString updateAlert READ getUpdateAlert CONSTANT);

public:
    explicit C_EVENT_NAMES(QObject *parent = nullptr) : QObject(parent) {};
    QString getInited() const { return m_inited; }
    QString getRequest() const { return m_request; }
    QString getUpdateAlert() const { return m_updateAlert; }

private:
    QString m_inited = "t_inited";
    QString m_request = "t_request";
    QString m_updateAlert = "t_updateAlert";
};

class LXSingleBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString version READ getVersion WRITE setVersion NOTIFY versionChanged);
    Q_PROPERTY(QString env READ getEnv WRITE setEnv NOTIFY envChanged);
    Q_PROPERTY(CurrentScriptInfo* currentScriptInfo READ getCurrentScriptInfo CONSTANT);
    Q_PROPERTY(C_EVENT_NAMES* EVENT_NAMES READ getEventNames CONSTANT);

public:
    explicit LXSingleBridge(QJSEngine* engine, QObject *parent = nullptr);

    QString getVersion() const { return m_version; }
    void setVersion(const QString &version) {
        if (m_version != version) {
            m_version = version;
            emit versionChanged();
        }
    }

    QString getEnv() const { return m_env; }
    void setEnv(const QString &env) {
        if (m_env != env) {
            m_env = env;
            emit envChanged();
        }
    }

    CurrentScriptInfo* getCurrentScriptInfo() const { return m_currentScriptInfo; }
    C_EVENT_NAMES* getEventNames() const { return m_EVENT_NAMES; }
    Q_INVOKABLE void on(QString eventName, std::function<void()> handler){
        qDebug() << QString("脚本注册事件：%1").arg(eventName);
        eventHandlers.insert(eventName, handler);
    }

    Q_INVOKABLE void send(QString eventName, QJSValue data){
        qDebug() << QString("脚本发送事件：%1，数据：%2").arg(eventName).arg(data.toString());
        if (eventName == getEventNames()->getInited()) {
            isInited = true;
        } 
    }

    Q_INVOKABLE void request(QString url, QJSValue options, QJSValue callback) {
        qDebug() << QString("脚本请求网络：%1").arg(url);
        m_netRequest->request(url, options, callback);
    }

signals:
    void versionChanged();
    void envChanged();

private:
    QString m_version = "1.0";
    QString m_env = "desktop";
    CurrentScriptInfo* m_currentScriptInfo;
    C_EVENT_NAMES* m_EVENT_NAMES;

    QHash<QString, std::function<void()>> eventHandlers;
    bool isInited = false;

    LXNetRequest* m_netRequest = nullptr;
    QJSEngine* m_engine = nullptr;

};      

