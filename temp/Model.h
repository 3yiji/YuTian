// Model.h
#pragma once
#include <QObject>
#include <QDebug>

class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(int counter READ counter WRITE setCounter NOTIFY counterChanged)
public:
    Model(QObject *parent = nullptr) : QObject(parent), m_counter(110) {}

    int counter() const { return m_counter; }

public slots:
    void setCounter(int value) {
        if (m_counter == value) return;
        m_counter = value;
        qDebug() << "m_counter changed to:" << m_counter;
        emit counterChanged();
    }

signals:
    void counterChanged();

private:
    int m_counter;
};
