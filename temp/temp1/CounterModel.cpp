#include "CounterModel.h"

CounterModel::CounterModel(QObject* parent) : QObject(parent) {}

int CounterModel::value() const {
    return m_value;
}

void CounterModel::increment() {
    m_value++;
}

void CounterModel::reset() {
    m_value = 0;
} 