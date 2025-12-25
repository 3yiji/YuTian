#pragma once
#include <QObject>

class CounterModel;

class CounterViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int value READ value NOTIFY valueChanged)
    
public:
    explicit CounterViewModel(CounterModel* model, QObject* parent = nullptr);
    
    int value() const;
    
    Q_INVOKABLE void increment();
    Q_INVOKABLE void reset();

signals:
    void valueChanged(int newValue);

private:
    CounterModel* model;
}; 