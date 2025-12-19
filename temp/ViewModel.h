// ViewModel.h
#pragma once
#include <QObject>
#include "Model.h"

class ViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int counter5 READ counter NOTIFY counterChanged)
public:
    ViewModel(QObject *parent = nullptr) : QObject(parent), m_model(new Model(this)) {}

    // int counter() const { return m_model->counter(); }
    int counter() const {
        int value = m_model->counter();
        return value;
    }

public slots:
    void increment() {
        m_model->setCounter(m_model->counter() + 1);
        emit counterChanged();
    }

signals:
    void counterChanged();

private:
    Model *m_model;
};
