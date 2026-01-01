#include "LXSingleBridge.h"
#include <QJSEngine>
#include <QJSValue>

LXSingleBridge::LXSingleBridge(QJSEngine* m_engine, QObject *parent)
    : QObject(parent)
{
    this->m_engine = m_engine;
    m_currentScriptInfo = new CurrentScriptInfo(this);
    m_EVENT_NAMES = new C_EVENT_NAMES(this);
    m_netRequest = new LXNetRequest(m_engine, this);
}

