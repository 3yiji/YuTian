#include "CounterViewModel.h"
#include "CounterModel.h"

CounterViewModel::CounterViewModel(CounterModel* model, QObject* parent)
    : QObject(parent), model(model) {
    
    // 监听模型变化并转发
    connect(model, &CounterModel::valueChanged, this, [this] {
        emit valueChanged(model->value());
    });
}

int CounterViewModel::value() const {
    return model->value();
}

void CounterViewModel::increment() {
    model->increment();
    emit valueChanged(model->value());
}

void CounterViewModel::reset() {
    model->reset();
    emit valueChanged(model->value());
} 