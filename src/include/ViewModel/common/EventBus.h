#pragma once
#include <QObject>

class EventBus : public QObject
{
    Q_OBJECT
public:
    static EventBus* instance() {
        static EventBus inst;
        return &inst;
    }

signals:
    void searchRequested(const QString &keyword);

private:
    EventBus() = default;
};