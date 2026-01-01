#include "LXNetRequest.h"
#include <QDebug>
#include <QBuffer>
#include <QJSValueIterator>

// 构造函数初始化网络管理器
LXNetRequest::LXNetRequest(QJSEngine* engine, QObject *parent) : QObject(parent)
{
    m_engine = engine;
    m_networkManager = new QNetworkAccessManager(this);
    
    // 连接网络信号槽
    connect(m_networkManager, &QNetworkAccessManager::finished, 
            this, &LXNetRequest::onReplyFinished);
}

// 析构函数清理资源
LXNetRequest::~LXNetRequest()
{
    // 取消所有未完成的请求
    for (auto& ctx : m_requestContexts) {
        if (ctx.reply && ctx.reply->isRunning()) {
            ctx.reply->abort();
        }
        if (ctx.timeoutTimer) {
            ctx.timeoutTimer->stop();
            ctx.timeoutTimer->deleteLater();
        }
        if (ctx.multiPart) {
            ctx.multiPart->deleteLater();
        }
    }
    m_requestContexts.clear();
}

// 解析 JS 的 options 参数（复刻 JS 的参数初始化逻辑）
void LXNetRequest::parseOptions(const QJSValue& options, QString& method, 
                                 QVariantMap& headers, int& timeout, 
                                 QVariantMap& formData, QVariantMap& form, QJSValue& body)
{
    // 默认值初始化
    method = "GET";
    timeout = 10000;
    headers.clear();
    formData.clear();
    form.clear();
    body = QJSValue(QJSValue::UndefinedValue);

    if (!options.isObject()) return;

    // 1. 解析 method
    if (options.hasProperty("method")) {
        method = options.property("method").toString().toUpper();
    }

    // 2. 解析 timeout
    if (options.hasProperty("timeout") && options.property("timeout").isNumber()) {
        timeout = options.property("timeout").toInt();
    }

    // 3. 解析 headers
    if (options.hasProperty("headers") && options.property("headers").isObject()) {
        QJSValue headersObj = options.property("headers");
        QJSValueIterator it(headersObj);
        while (it.hasNext()) {
            it.next();
            headers.insert(it.name(), it.value().toString());
        }
    }

    // 4. 解析 formData
    if (options.hasProperty("formData") && options.property("formData").isObject()) {
        QJSValue formDataObj = options.property("formData");
        QJSValueIterator it(formDataObj);
        while (it.hasNext()) {
            it.next();
            formData.insert(it.name(), it.value().toString());
        }
    }

    // 5. 解析 form
    if (options.hasProperty("form") && options.property("form").isObject()) {
        QJSValue formObj = options.property("form");
        QJSValueIterator it(formObj);
        while (it.hasNext()) {
            it.next();
            form.insert(it.name(), it.value().toString());
        }
    }

    // 6. 解析 body
    if (options.hasProperty("body")) {
        body = options.property("body");
    }
}

// 构建网络请求（处理请求头）
QNetworkRequest LXNetRequest::buildNetworkRequest(const QString& url, const QVariantMap& headers)
{
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    // 设置请求头
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toString().toUtf8());
    }
    return request;
}

// 调用 JS 回调函数（封装回调逻辑）
void LXNetRequest::callJsCallback(const QJSValue& callback, const QString& error, const QJSValue& response)
{
    if (!callback.isCallable()) return;

    QJSValueList args;
    if (!error.isEmpty()) {
        // 错误：第一个参数为 Error 对象，第二个为 null
        QJSValue errorObj = m_engine->newErrorObject(QJSValue::GenericError, error);
        args << errorObj << QJSValue(QJSValue::NullValue);
    } else {
        // 成功：第一个参数为 null，第二个为 response 对象
        args << QJSValue(QJSValue::NullValue) << response;
    }

    // 异步调用回调（避免阻塞事件循环）
    QMetaObject::invokeMethod(this, [=]() {
        callback.call(args);
    }, Qt::QueuedConnection);
}

// ========== 核心：复刻 lx-request.js 的 request 方法 ==========
QJSValue LXNetRequest::request(QString url, QJSValue options, QJSValue callback)
{
    // 0. 校验参数
    if (!callback.isCallable()) {
        qWarning() << "request 回调函数必须是可调用的！";
        return QJSValue(QJSValue::UndefinedValue);
    }

    // 2. 解析 options 参数
    QString method;
    QVariantMap headers;
    int timeout;
    QVariantMap formData;
    QVariantMap form;
    QJSValue body;
    parseOptions(options, method, headers, timeout, formData, form, body);

    // 3. 输出请求发起日志（复刻 JS 的日志格式）
    // log(requestId, QString("脚本请求 URL：%1，参数：%2").arg(url).arg(options.toString()));

    // 4. 处理请求体（复刻 JS 的 formData/form/body 逻辑）
    QNetworkRequest networkRequest = buildNetworkRequest(url, headers);
    QByteArray requestBody;
    QHttpMultiPart* multiPart = nullptr;

    if (method != "GET" && method != "HEAD") {
        // 4.1 处理 formData（对应 JS 的 FormData）
        if (!formData.isEmpty()) {
            multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
            
            for (auto it = formData.begin(); it != formData.end(); ++it) {
                QString key = it.key();
                QVariant value = it.value();

                QHttpPart part;
                part.setHeader(QNetworkRequest::ContentDispositionHeader,
                               QString("form-data; name=\"%1\"").arg(key));
                if (value.type() == QVariant::ByteArray) {
                    part.setBody(value.toByteArray());
                } else {
                    part.setBody(value.toString().toUtf8());
                }
                multiPart->append(part);
            }

            // 设置 formData 对应的请求头
            // networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, multiPart->contentType());
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=%1").arg(QString(multiPart->boundary())));
        }
        // 4.2 处理 form（对应 JS 的 URLSearchParams）
        else if (!form.isEmpty()) {
            QUrlQuery query;
            
            for (auto it = form.begin(); it != form.end(); ++it) {
                QString key = it.key();
                QString value = it.value().toString();
                query.addQueryItem(key, value);
            }

            requestBody = query.toString(QUrl::FullyEncoded).toUtf8();
            if (!networkRequest.hasRawHeader("Content-Type")) {
                networkRequest.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
            }
        }
        // 4.3 处理 body（JSON/Buffer/文本）
        else if (!body.isUndefined()) {
            if (body.isObject() && !body.isArray() && !body.isDate() && !body.isRegExp()) {
                // JSON 类型（对应 JS 的 JSON.stringify）
                QJsonDocument jsonDoc = QJsonDocument::fromVariant(body.toVariant());
                requestBody = jsonDoc.toJson(QJsonDocument::Indented);
                if (!networkRequest.hasRawHeader("Content-Type")) {
                    networkRequest.setRawHeader("Content-Type", "application/json");
                }
            } else if (body.isVariant() && body.toVariant().canConvert<QByteArray>()) {
                // Buffer 类型（对应 JS 的 Buffer）
                requestBody = body.toVariant().toByteArray();
            } else {
                // 普通文本
                requestBody = body.toString().toUtf8();
            }
        }
        // 4.4 无请求体
        else {
            qDebug() << "无请求体";
        }
    } else {
        qDebug() << "无请求体（GET/HEAD 方法）";
    }

    // 5. 发送网络请求
    QNetworkReply* reply = nullptr;
    if (method == "GET") {
        reply = m_networkManager->get(networkRequest);
    } else if (method == "POST") {
        if (multiPart) {
            reply = m_networkManager->post(networkRequest, multiPart);
            multiPart->setParent(reply); // 让 reply 管理 multiPart 生命周期
        } else {
            reply = m_networkManager->post(networkRequest, requestBody);
        }
    } else if (method == "PUT") {
        reply = m_networkManager->put(networkRequest, requestBody);
    } else if (method == "DELETE") {
        reply = m_networkManager->deleteResource(networkRequest);
    } else {
        reply = m_networkManager->sendCustomRequest(networkRequest, method.toUtf8(), requestBody);
    }

    // 6. 设置超时定时器（复刻 JS 的 timeout 逻辑）
    QTimer* timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(timeout);
    connect(timeoutTimer, &QTimer::timeout, this, &LXNetRequest::onRequestTimeout, Qt::SingleShotConnection);
    timeoutTimer->start();

    // 8. 存储请求上下文
    RequestContext ctx;
    ctx.reply = reply;
    ctx.timeoutTimer = timeoutTimer;
    ctx.callback = callback;
    ctx.multiPart = multiPart;
    m_requestContexts.insert(reply, ctx);

    // 9. 绑定错误信号
    connect(reply, &QNetworkReply::errorOccurred, this, &LXNetRequest::onReplyError, Qt::SingleShotConnection);

    // 10. 返回取消请求的函数（复刻 JS 的 cancelRequest）
    // CancelRequest* cancelRequest = new CancelRequest(reply, this);
    // // std::function<void()> func = cancelRequest->call;
    // std::function<void()> func = [cancelRequest]() { cancelRequest->call(); };
    // return func;

    // return m_engine->newFunction([this, reply](const QJSValueList&) {
    //     if (m_requestContexts.contains(reply) && reply->isRunning()) {
    //         reply->abort();
    //         qDebug() << "请求已被取消";
            
    //     }
    //     return QJSValue();
    // });

    // return QJSValue([this, reply](const QJSValueList&) {
    //     if (m_requestContexts.contains(reply) && reply->isRunning()) {
    //         reply->abort();
    //         qDebug() << "请求已被取消";
            
    //     }
    //     return QJSValue();
    // });
    return QJSValue();
}

// ========== 网络请求响应处理 ==========
void LXNetRequest::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_requestContexts.contains(reply)) return;

    // 1. 获取请求上下文
    RequestContext ctx = m_requestContexts.take(reply);
    ctx.timeoutTimer->stop();
    ctx.timeoutTimer->deleteLater();
    QString requestId = ctx.requestId;

    // 2. 解析响应数据（复刻 JS 的响应处理逻辑）
    QJSValue responseObj = m_engine->newObject();
    // 响应状态
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString statusMessage = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    responseObj.setProperty("statusCode", statusCode);
    responseObj.setProperty("statusMessage", statusMessage);

    // 响应头
    QJSValue headersObj = m_engine->newObject();
    for (auto it = reply->rawHeaderPairs().begin(); it != reply->rawHeaderPairs().end(); ++it) {
        QString key = QString(it->first);
        QString value = QString(it->second);
        headersObj.setProperty(key, value);
    }
    responseObj.setProperty("headers", headersObj);

    // 响应体
    QByteArray responseBody = reply->readAll();
    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QJSValue bodyValue;

    if (contentType.contains("application/json") || contentType.contains("text/")) {
        // JSON/文本响应体
        bodyValue = m_engine->evaluate(QString("JSON.parse('%1')").arg(QString(responseBody).replace("'", "\\'")));
        if (bodyValue.isError()) {
            bodyValue = QString(responseBody);
        }
        
    } else if (contentType.contains("image/") || contentType.contains("application/octet-stream")) {
        // 二进制响应体（对应 JS 的 Buffer）
        // bodyValue = m_engine->newArrayBuffer(responseBody);
        QJSValue arrayBuffer = m_engine->evaluate(QString("new ArrayBuffer(%1)").arg(responseBody.size()));
        QJSValue uint8Array = m_engine->globalObject().property("Uint8Array").callAsConstructor(QJSValueList() << arrayBuffer);
        for (int i = 0; i < responseBody.size(); ++i) {
            uint8Array.setProperty(i, QJSValue((int)(unsigned char)responseBody[i]));
        }
        bodyValue = arrayBuffer;
    } else {
        // 其他类型
        bodyValue = QString(responseBody);
    }
    responseObj.setProperty("body", bodyValue);

    // 4. 调用 JS 回调（返回成功结果）
    callJsCallback(ctx.callback, "", responseObj);

    // 5. 清理资源
    reply->deleteLater();
}

// ========== 请求超时处理 ==========
void LXNetRequest::onRequestTimeout()
{
    QTimer* timer = qobject_cast<QTimer*>(sender());
    if (!timer) return;

    // 查找对应的请求上下文
    QNetworkReply* reply = nullptr;
    RequestContext ctx;
    for (auto it = m_requestContexts.begin(); it != m_requestContexts.end(); ++it) {
        if (it.value().timeoutTimer == timer) {
            reply = it.key();
            ctx = it.value();
            break;
        }
    }

    if (!reply) return;

    // 取消请求
    reply->abort();
    m_requestContexts.take(reply);
    QString errorMsg = QString("RequestTimeout: 请求超时（%1ms）").arg(timer->interval());

    // 调用 JS 回调（返回超时错误）
    callJsCallback(ctx.callback, errorMsg, QJSValue(QJSValue::NullValue));

    // 清理资源
    timer->deleteLater();
    reply->deleteLater();
}

// ========== 请求错误处理 ==========
void LXNetRequest::onReplyError(QNetworkReply::NetworkError error)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_requestContexts.contains(reply)) return;

    // 1. 获取请求上下文
    RequestContext ctx = m_requestContexts.take(reply);
    ctx.timeoutTimer->stop();
    ctx.timeoutTimer->deleteLater();
    QString requestId = ctx.requestId;

    // 2. 解析错误类型（复刻 JS 的错误分类）
    QString errorType, errorMessage;
    switch (error) {
        case QNetworkReply::TimeoutError:
            errorType = "RequestTimeout";
            errorMessage = QString("请求超时（%1ms）").arg(ctx.timeoutTimer->interval());
            break;
        case QNetworkReply::HostNotFoundError:
            errorType = "DNSNotFound";
            errorMessage = QString("DNS 解析失败: %1").arg(reply->url().host());
            break;
        case QNetworkReply::ConnectionRefusedError:
            errorType = "ConnectionRefused";
            errorMessage = QString("连接被拒绝: %1").arg(reply->url().toString());
            break;
        case QNetworkReply::OperationCanceledError:
            errorType = "RequestAborted";
            errorMessage = "请求被取消";
            break;
        default:
            errorType = "UnknownError";
            errorMessage = reply->errorString();
            break;
    }

    // 4. 调用 JS 回调（返回错误）
    callJsCallback(ctx.callback, QString("%1: %2").arg(errorType).arg(errorMessage), QJSValue(QJSValue::NullValue));

    // 5. 清理资源
    reply->deleteLater();
}

