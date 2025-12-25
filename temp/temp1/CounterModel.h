#pragma once
#include <QObject>

class CounterModel : public QObject {
    Q_OBJECT
public:
    explicit CounterModel(QObject* parent = nullptr);
    int value() const;
    void increment();
    void reset();

private:
    int m_value = 0;
}; 