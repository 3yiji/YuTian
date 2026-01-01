#pragma once

#include <QObject>
#include <QByteArray>
#include <QCryptographicHash>
#include <QJSEngine>
#include <QJSValue>
// ========== 新增网络相关头文件 ==========
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <functional>

// 请求上下文（存储单次请求的所有信息）
struct RequestContext {
    QString requestId;          // 请求唯一ID
    QNetworkReply* reply = nullptr; // 网络请求对象
    QTimer* timeoutTimer = nullptr; // 超时定时器
    QJSValue callback;          // JS 回调函数
    QHttpMultiPart* multiPart = nullptr; // formData 专用，避免内存泄漏
};

// 单个类实现所有嵌套层级的逻辑（包含 request 方法）
class LXNetRequest : public QObject
{
    Q_OBJECT
public:
    explicit LXNetRequest(QJSEngine* engine, QObject *parent = nullptr);
    ~LXNetRequest   (); // 析构函数清理资源

    // ========== 复刻 lx-request.js 的核心方法 ==========
    Q_INVOKABLE QJSValue request(QString url, QJSValue options, QJSValue callback);

private slots:
    // 网络请求响应处理
    void onReplyFinished();
    // 请求超时处理
    void onRequestTimeout();
    // 请求错误处理
    void onReplyError(QNetworkReply::NetworkError error);

private:
    // ========== 工具方法 ==========
    
    // 解析 JS 的 options 为 Qt 可识别的参数
    void parseOptions(const QJSValue& options, QString& method, QVariantMap& headers, 
                      int& timeout, QVariantMap& formData, QVariantMap& form, QJSValue& body);
    // 构建网络请求（处理不同请求体类型）
    QNetworkRequest buildNetworkRequest(const QString& url, const QVariantMap& headers);
    // 调用 JS 回调函数
    void callJsCallback(const QJSValue& callback, const QString& error, const QJSValue& response);

    // ========== 成员变量 ==========
    QNetworkAccessManager* m_networkManager; // 网络请求管理器
    QMap<QNetworkReply*, RequestContext> m_requestContexts; // 请求上下文映射
    QJSEngine* m_engine = nullptr; // 保存 JS 引擎指针
};


class CancelRequest : public QObject {
    Q_OBJECT
public:
    CancelRequest(QNetworkReply* reply, QObject *parent = nullptr)
        : QObject(parent), reply_(reply) {}

    Q_INVOKABLE void call() {
        if (!aborted_) {
            aborted_ = true;
            qDebug() << reply_ << "已取消请求";
        }
    }

private:
    QNetworkReply* reply_;
    bool aborted_ = false;
};